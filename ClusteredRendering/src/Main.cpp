#include <DirectXPCH.h>
#include "GameTimer.h"
#include "Camera.h"
#include "InputManager.h"
#include "Keycodes.h"

// Define window & VSync Setting
unsigned __int16 g_windowWidth = 1280;
unsigned __int16 g_windowHeight = 720;
LPCSTR g_windowClassName = "DirectXWindowClass";
LPCSTR g_windowName = "Clustered Rendering Engine";
HWND g_windowHandle = 0;

const BOOL g_enableVSync = FALSE;

// Direct 3D device and swap chain
ID3D11Device* g_d3dDevice = nullptr;
ID3D11DeviceContext* g_d3dDeviceContext = nullptr;
IDXGISwapChain* g_d3dSwapChain = nullptr;

// Render target view for the back buffer of the swap chain
ID3D11RenderTargetView* g_d3dRenderTargetView = nullptr;
// Depth/stencil view for use as a depth buffer
ID3D11DepthStencilView* g_d3dDepthStencilView = nullptr;
// A texture to associate to the depth stencil view
ID3D11Texture2D* g_d3dDepthStencilBuffer = nullptr;

// Define the functionality of the depth/stencil stages
ID3D11DepthStencilState* g_d3dDepthStencilState = nullptr;
// Define the functionality of the rasterizer stage
ID3D11RasterizerState* g_d3dRasterizerState = nullptr;
D3D11_VIEWPORT g_viewport = { 0 };

// Vertex buffer data
ID3D11InputLayout* g_d3dInputLayout = nullptr;
ID3D11Buffer* g_d3dVertexBuffer = nullptr;
ID3D11Buffer* g_d3dIndexBuffer = nullptr;

// Shader data
ID3D11VertexShader* g_d3dVertexShader = nullptr;
ID3D11PixelShader* g_d3dPixelShader = nullptr;

float lastValue = 0;
float diff = 0;

// Shader resources 
enum ConstantBuffer
{
	CB_Application,
	CB_Frame,
	CB_Object,
	NumConstantBuffers
};

ID3D11Buffer* g_d3dConstantBuffers[NumConstantBuffers];

// Demo parameteres
XMMATRIX g_worldMatrix;
XMMATRIX g_viewMatrix;
XMMATRIX g_projectionMatrix;

// Vertex data for a colored cube
struct VertexPosColor
{
	XMFLOAT3 position;
	XMFLOAT3 color;
};

VertexPosColor g_vertices[8] =
{
	{ XMFLOAT3(-1.0f,-1.0f,-1.0f), XMFLOAT3(0.0f, 0.0f, 0.0f) }, // 0
	{ XMFLOAT3(-1.0f, 1.0f,-1.0f), XMFLOAT3(0.0f, 1.0f, 0.0f) }, // 1
	{ XMFLOAT3(1.0f, 1.0f,-1.0f), XMFLOAT3(1.0f, 1.0f, 0.0f) }, // 2
	{ XMFLOAT3(1.0f,-1.0f,-1.0f), XMFLOAT3(1.0f, 0.0f, 0.0f) }, // 3
	{ XMFLOAT3(-1.0f,-1.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f) }, // 4
	{ XMFLOAT3(-1.0f, 1.0f, 1.0f), XMFLOAT3(0.0f, 1.0f, 1.0f) }, // 5
	{ XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT3(1.0f, 1.0f, 1.0f) }, // 6
	{ XMFLOAT3(1.0f,-1.0f, 1.0f), XMFLOAT3(1.0f, 0.0f, 1.0f) }, // 7
};

WORD g_indicies[36] =
{
	0, 1, 2, 0, 2, 3,
	4, 6, 5, 4, 7, 6,
	4, 5, 1, 4, 1, 0,
	3, 2, 6, 3, 6, 7,
	1, 5, 6, 1, 6, 2,
	4, 0, 3, 4, 3, 7

};

GameTimer g_Timer;
Camera g_cam;
InputManager g_input;

// Forward declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

template< class ShaderClass >
ShaderClass* LoadShader(const std::wstring& filename, const std::string& entryPoint, const std::string& profile);

bool LoadContent();
void UnloadContent();

void Update(float deltaTime);
void Render();
void Cleanup();

void CaculateRenderStats();

// Intitialize the application window
int InitApplication(HINSTANCE hInstance, int cmdShow)
{
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = &WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = g_windowClassName;

	ShowCursor(false);

	if (!RegisterClassEx(&wndClass))
	{
		return -1;
	}

	RECT windowRect = { 0, 0, g_windowWidth, g_windowHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	g_windowHandle = CreateWindowA(g_windowClassName, g_windowName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);

	if (!g_windowHandle)
	{
		return -1;
	}

	ShowWindow(g_windowHandle, cmdShow);
	UpdateWindow(g_windowHandle);

	return 0;
}

DXGI_RATIONAL QueryRefreshRate(UINT screenWidth, UINT screenHeight, BOOL vsync)
{
	DXGI_RATIONAL refreshRate = { 0,1 };
	if (vsync)
	{
		IDXGIFactory* factory;
		IDXGIAdapter* adapter;
		IDXGIOutput* adapterOutput;
		DXGI_MODE_DESC* displayModeList;

		// Create a DirectX graphics interface factory
		HRESULT hr = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Could not create DXGIFactory instance"),
				TEXT("Error"),
				MB_OK);

			throw new std::exception("Failedto create DXGIFactory");
		}

		hr = factory->EnumAdapters(0, &adapter);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapters."),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to enumerate adapters.");
		}

		hr = adapter->EnumOutputs(0, &adapterOutput);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to enumerate adapter outputs"),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to enumerate adapter outputs");
		}

		UINT numDisplayModes;
		hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, nullptr);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list"),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to query display mode list");
		}

		displayModeList = new DXGI_MODE_DESC[numDisplayModes];
		assert(displayModeList);

		hr = adapterOutput->GetDisplayModeList(DXGI_FORMAT_B8G8R8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numDisplayModes, displayModeList);
		if (FAILED(hr))
		{
			MessageBox(0,
				TEXT("Failed to query display mode list"),
				TEXT("Query Refresh Rate"),
				MB_OK);

			throw new std::exception("Failed to query display mode list");
		}

		// Now store the refresh rate of the monitor that matches the width and height of the requested screen
		for (UINT i = 0; i < numDisplayModes; i++)
		{
			if (displayModeList[i].Width == screenWidth && displayModeList[i].Height == screenHeight)
			{
				refreshRate = displayModeList[i].RefreshRate;
			}
		}


		delete[] displayModeList;
		SafeRelease(adapterOutput);
		SafeRelease(adapter);
		SafeRelease(factory);
	}

	return refreshRate;
}

// Initialize the DirectX device and swap chain
int InitDirectX(HINSTANCE hInstance, BOOL vSync)
{
	// A window handle must have been created already.
	assert(g_windowHandle != 0);

	RECT clientRect;
	GetClientRect(g_windowHandle, &clientRect);

	// Compute the exact client dimensions. This will be used
	// to initialize the render targets for our swap chain.
	unsigned int clientWidth = clientRect.right - clientRect.left;
	unsigned int clientHeight = clientRect.bottom - clientRect.top;

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	swapChainDesc.BufferCount = 1;
	swapChainDesc.BufferDesc.Width = clientWidth;
	swapChainDesc.BufferDesc.Height = clientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate = QueryRefreshRate(clientWidth, clientHeight, vSync);
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = g_windowHandle;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	swapChainDesc.Windowed = TRUE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	UINT createDeviceFlags = 0;
#if _DEBUG
	createDeviceFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	// These are the feature levels that we will accept.
	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
		D3D_FEATURE_LEVEL_9_3,
		D3D_FEATURE_LEVEL_9_2,
		D3D_FEATURE_LEVEL_9_1
	};

	// This will be the feature level that 
	// is used to create our device and swap chain.
	D3D_FEATURE_LEVEL featureLevel;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
		nullptr, createDeviceFlags, featureLevels, _countof(featureLevels),
		D3D11_SDK_VERSION, &swapChainDesc, &g_d3dSwapChain, &g_d3dDevice, &featureLevel,
		&g_d3dDeviceContext);

	if (hr == E_INVALIDARG)
	{
		hr = D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE,
			nullptr, createDeviceFlags, &featureLevels[1], _countof(featureLevels) - 1,
			D3D11_SDK_VERSION, &swapChainDesc, &g_d3dSwapChain, &g_d3dDevice, &featureLevel,
			&g_d3dDeviceContext);
	}

   	if (FAILED(hr))
	{
		return -1;
	}

	// The Direct3D device and swap chain were successfully created.
	// Now we need to initialize the buffers of the swap chain.
	// Next initialize the back buffer of the swap chain and associate it to a 
	// render target view.
	ID3D11Texture2D* backBuffer;
	hr = g_d3dSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
	if (FAILED(hr))
	{
		return -1;
	}

	hr = g_d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &g_d3dRenderTargetView);
	if (FAILED(hr))
	{
		return -1;
	}

	SafeRelease(backBuffer);

	// Create the depth buffer for use with the depth/stencil view.
	D3D11_TEXTURE2D_DESC depthStencilBufferDesc;
	ZeroMemory(&depthStencilBufferDesc, sizeof(D3D11_TEXTURE2D_DESC));

	depthStencilBufferDesc.ArraySize = 1;
	depthStencilBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilBufferDesc.CPUAccessFlags = 0; // No CPU access required.
	depthStencilBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilBufferDesc.Width = clientWidth;
	depthStencilBufferDesc.Height = clientHeight;
	depthStencilBufferDesc.MipLevels = 1;
	depthStencilBufferDesc.SampleDesc.Count = 1;
	depthStencilBufferDesc.SampleDesc.Quality = 0;
	depthStencilBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = g_d3dDevice->CreateTexture2D(&depthStencilBufferDesc, nullptr, &g_d3dDepthStencilBuffer);
	if (FAILED(hr))
	{
		return -1;
	}

	hr = g_d3dDevice->CreateDepthStencilView(g_d3dDepthStencilBuffer, nullptr, &g_d3dDepthStencilView);
	if (FAILED(hr))
	{
		return -1;
	}

	// Setup depth/stencil state.
	D3D11_DEPTH_STENCIL_DESC depthStencilStateDesc;
	ZeroMemory(&depthStencilStateDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));

	depthStencilStateDesc.DepthEnable = TRUE;
	depthStencilStateDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilStateDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilStateDesc.StencilEnable = FALSE;

	hr = g_d3dDevice->CreateDepthStencilState(&depthStencilStateDesc, &g_d3dDepthStencilState);

	// Setup rasterizer state.
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.ScissorEnable = FALSE;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state object.
	hr = g_d3dDevice->CreateRasterizerState(&rasterizerDesc, &g_d3dRasterizerState);
	if (FAILED(hr))
	{
		return -1;
	}

	// Initialize the viewport to occupy the entire client area.
	g_viewport.Width = static_cast<float>(clientWidth);
	g_viewport.Height = static_cast<float>(clientHeight);
	g_viewport.TopLeftX = 0.0f;
	g_viewport.TopLeftY = 0.0f;
	g_viewport.MinDepth = 0.0f;
	g_viewport.MaxDepth = 1.0f;

	return 0;
}

bool LoadContent()
{
	assert(g_d3dDevice);

	// Create an initialize the vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = sizeof(VertexPosColor) * _countof(g_vertices);
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	resourceData.pSysMem = g_vertices;

	HRESULT hr = g_d3dDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &g_d3dVertexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Create and initialize the index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(WORD) * _countof(g_indicies);
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	resourceData.pSysMem = g_indicies;

	hr = g_d3dDevice->CreateBuffer(&indexBufferDesc, &resourceData, &g_d3dIndexBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Create the constant buffers for the variables defined in the vertex shader.
	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(XMMATRIX);
	constantBufferDesc.CPUAccessFlags = 0;
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;

	hr = g_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffers[CB_Application]);
	if (FAILED(hr))
	{
		return false;
	}
	hr = g_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffers[CB_Frame]);
	if (FAILED(hr))
	{
		return false;
	}
	hr = g_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dConstantBuffers[CB_Object]);
	if (FAILED(hr))
	{
		return false;
	}

	// Load the shaders
	//g_d3dVertexShader = LoadShader<ID3D11VertexShader>( L"../data/shaders/SimpleVertexShader.hlsl", "SimpleVertexShader", "latest" );
	//g_d3dPixelShader = LoadShader<ID3D11PixelShader>( L"../data/shaders/SimplePixelShader.hlsl", "SimplePixelShader", "latest" );

	// Load the compiled vertex shader.
	ID3DBlob* vertexShaderBlob;
#if _DEBUG
	LPCWSTR compiledVertexShaderObject = L"SimpleVertexShader_d.cso";
#else
	LPCWSTR compiledVertexShaderObject = L"SimpleVertexShader.cso";
#endif

	hr = D3DReadFileToBlob(compiledVertexShaderObject, &vertexShaderBlob);
	if (FAILED(hr))
	{
		return false;
	}

	hr = g_d3dDevice->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), nullptr, &g_d3dVertexShader);
	if (FAILED(hr))
	{
		return false;
	}

	// Create the input layout for the vertex shader.
	D3D11_INPUT_ELEMENT_DESC vertexLayoutDesc[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof(VertexPosColor,color), D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	hr = g_d3dDevice->CreateInputLayout(vertexLayoutDesc, _countof(vertexLayoutDesc), vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), &g_d3dInputLayout);
	if (FAILED(hr))
	{
		return false;
	}

	SafeRelease(vertexShaderBlob);

	// Load the compiled pixel shader.
	ID3DBlob* pixelShaderBlob;
#if _DEBUG
	LPCWSTR compiledPixelShaderObject = L"SimplePixelShader_d.cso";
#else
	LPCWSTR compiledPixelShaderObject = L"SimplePixelShader.cso";
#endif

	hr = D3DReadFileToBlob(compiledPixelShaderObject, &pixelShaderBlob);
	if (FAILED(hr))
	{
		return false;
	}

	hr = g_d3dDevice->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), nullptr, &g_d3dPixelShader);
	if (FAILED(hr))
	{
		return false;
	}

	SafeRelease(pixelShaderBlob);

	// Setup the projection matrix.
	RECT clientRect;
	GetClientRect(g_windowHandle, &clientRect);

	// Compute the exact client dimensions.
	// This is required for a correct projection matrix.
	float clientWidth = static_cast<float>(clientRect.right - clientRect.left);
	float clientHeight = static_cast<float>(clientRect.bottom - clientRect.top);

	g_cam.SetLens(XMConvertToRadians(68.0f), 0.1f, 100.0f, static_cast<unsigned int>(clientWidth), static_cast<unsigned int>(clientHeight));

	g_projectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(45.0f), clientWidth / clientHeight, 0.1f, 100.0f);

	g_d3dDeviceContext->UpdateSubresource(g_d3dConstantBuffers[CB_Application], 0, nullptr, &g_cam.GetCamData().projMat, 0, 0);

	return true;
}

// Get the lates profile for the specified shader type
template<class ShaderClass>
std::string GetLatestProfile();

template<>
std::string GetLatestProfile<ID3D11VertexShader>()
{
	assert(g_d3dDevice);

	// Query the current feature level
	D3D_FEATURE_LEVEL featureLevel = g_d3dDevice->GetFeatureLevel();

	switch (featureLevel)
	{
	case D3D_FEATURE_LEVEL_11_1:
	{
		return "vs_5_1";
	}
	break;
	case D3D_FEATURE_LEVEL_11_0:
	{
		return "vs_5_0";
	}
	break;
	case D3D_FEATURE_LEVEL_10_1:
	{
		return "vs_4_1";
	}
	break;
	case D3D_FEATURE_LEVEL_10_0:
	{
		return "vs_4_0";
	}
	break;
	case D3D_FEATURE_LEVEL_9_3:
	{
		return "vs_4_0_level_9_3";
	}
	case D3D_FEATURE_LEVEL_9_2:
	case D3D_FEATURE_LEVEL_9_1:
	{
		return "vs_4_0_level_9_1";
	}
	break;
	}// switch( featureLevel)

	return "";
}

template <>
std::string GetLatestProfile<ID3D11PixelShader>()
{
	assert(g_d3dDevice);

	// Query the current feature level
	D3D_FEATURE_LEVEL featureLevel = g_d3dDevice->GetFeatureLevel();

	switch (featureLevel)
	{
	case D3D_FEATURE_LEVEL_11_1:
	{
		return "ps_5_1";
	}
	break;
	case D3D_FEATURE_LEVEL_11_0:
	{
		return "ps_5_0";
	}
	break;
	case D3D_FEATURE_LEVEL_10_1:
	{
		return "ps_4_1";
	}
	break;
	case D3D_FEATURE_LEVEL_10_0:
	{
		return "ps_4_0";
	}
	break;
	case D3D_FEATURE_LEVEL_9_3:
	{
		return "ps_4_0_level_9_3";
	}
	case D3D_FEATURE_LEVEL_9_2:
	case D3D_FEATURE_LEVEL_9_1:
	{
		return "ps_4_0_level_9_1";
	}
	break;
	}// switch( featureLevel)

	return "";
}

template < class ShaderClass >
ShaderClass* CreateShader(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage);

template<>
ID3D11VertexShader* CreateShader<ID3D11VertexShader>(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage)
{
	assert(g_d3dDevice);
	assert(pShaderBlob);

	ID3D11VertexShader* pVertexShader = nullptr;
	g_d3dDevice->CreateVertexShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pClassLinkage, &pVertexShader);

	return pVertexShader;
}

template<>
ID3D11PixelShader* CreateShader<ID3D11PixelShader>(ID3DBlob* pShaderBlob, ID3D11ClassLinkage* pClassLinkage)
{
	assert(g_d3dDevice);
	assert(pShaderBlob);

	ID3D11PixelShader* pPixelShader = nullptr;
	g_d3dDevice->CreatePixelShader(pShaderBlob->GetBufferPointer(), pShaderBlob->GetBufferSize(), pClassLinkage, &pPixelShader);

	return pPixelShader;
}

template< class ShaderClass >
ShaderClass* LoadShader(const std::wstring& fileName, const std::string& entryPoint, const std::string& _profile)
{
	ID3DBlob* pShaderBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;
	ShaderClass* pShader = nullptr;

	std::string profile = _profile;
	if (profile == "latest")
	{
		profile = GetLatestProfile<ShaderClass>();
	}

	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;
#if _DEBUG
	flags |= D3DCOMPILE_DEBUG;
#endif

	HRESULT hr = D3DCompileFromFile(fileName.c_str(), nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint.c_str(), profile.c_str(),
		flags, 0, &pShaderBlob, &pErrorBlob);

	if (FAILED(hr))
	{
		if (pErrorBlob)
		{
			std::string errorMessage = (char*)pErrorBlob->GetBufferPointer();
			OutputDebugStringA(errorMessage.c_str());

			SafeRelease(pShaderBlob);
			SafeRelease(pErrorBlob);
		}

		return false;
	}

	pShader = CreateShader<ShaderClass>(pShaderBlob, nullptr);

	SafeRelease(pShaderBlob);
	SafeRelease(pErrorBlob);

	return pShader;
}

void UnloadContent()
{
	SafeRelease(g_d3dConstantBuffers[CB_Application]);
	SafeRelease(g_d3dConstantBuffers[CB_Frame]);
	SafeRelease(g_d3dConstantBuffers[CB_Object]);
	SafeRelease(g_d3dIndexBuffer);
	SafeRelease(g_d3dVertexBuffer);
	SafeRelease(g_d3dInputLayout);
	SafeRelease(g_d3dVertexShader);
	SafeRelease(g_d3dPixelShader);

}

// Main application loop
int Run()
{
	MSG msg = { 0 };

	g_Timer.Reset();

	static DWORD previousTime = timeGetTime();
	static const float targetFramerate = 60.0f;
	static const float maxTimeStep = 1.0f / targetFramerate;

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			g_Timer.Tick();
			/*DWORD currentTime = timeGetTime();
			float deltaTime = (currentTime - previousTime) / 1000.0f;
			previousTime = currentTime;
			*/
			// Cap the delta time to the max time step ( useful if your debugging
			//and you dont want the deltaTime to explode
			//deltaTime = std::min<float>(deltaTime, maxTimeStep);
			CaculateRenderStats();
			Update(g_Timer.DeltaTime());
			Render();

		}
	}

	return static_cast<int>(msg.wParam);
}

// Windows main entry point
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPWSTR cmdLine, int cmdShow)
{
	UNREFERENCED_PARAMETER(prevInstance);
	UNREFERENCED_PARAMETER(cmdLine);

	// Check for DirectX Math library support
	if (!XMVerifyCPUSupport())
	{
		MessageBox(nullptr, TEXT("Failed to verify DirectX Math library support."), TEXT("Error"), MB_OK);
		return -1;
	}

	if (InitApplication(hInstance, cmdShow) != 0)
	{
		MessageBox(nullptr, TEXT("Failed to create application window."), TEXT("Error"), MB_OK);
		return -1;
	}

	if (InitDirectX(hInstance, g_enableVSync) != 0)
	{
		MessageBox(nullptr, TEXT("Failed to create DirectX device and swap chain"), TEXT("Error"), MB_OK);
		return -1;
	}

	if (!LoadContent())
	{
		MessageBox(nullptr, TEXT("Failed to load content"), TEXT("Error"), MB_OK);
		return -1;
	}

	int returnCode = Run();

	UnloadContent();
	Cleanup();

	return returnCode;
}

// Windows Procedure CallBack
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDC;

	switch (message)
	{
	case WM_CREATE:
	{

		g_input.InitializeRawMouse(hwnd);
	}
	break;
	case WM_KEYDOWN:
	{
		g_input.SetKeyState(wParam, true);

		if (g_input.KeyPressed(VK_ESCAPE))
		{
			PostMessage(hwnd, WM_QUIT, 0, 0);
		}
	}
	break;
	case WM_KEYUP:
	{
		g_input.SetKeyState(wParam, false);
	}
	break;
	case WM_INPUT:
	{
		
		UINT bufferSize;
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (LPVOID)g_input.GetMouseBuffer(), &bufferSize, sizeof(RAWINPUTHEADER));

		g_input.UpdateMouse();
		
	}
	break;
	case WM_PAINT:
	{
		hDC = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

void Update(float deltaTime)
{
	
	if (g_input.Initialized())
	{
		if (g_input.KeyDown(VK_W))
		{
			g_cam.MoveForward(deltaTime);
		}

		if (g_input.KeyDown(VK_S))
		{
			g_cam.MoveBackward(deltaTime);
		}

		if (g_input.KeyDown(VK_A))
		{
			g_cam.MoveLeft(deltaTime);
		}

		if (g_input.KeyDown(VK_D))
		{
			g_cam.MoveRight(deltaTime);
		}

		if (g_input.MouseScrolled())
		{
			if (g_input.GetMouseScrollPos() < 0)
			{
				g_cam.MoveBackward(deltaTime * 100);
			}
			if (g_input.GetMouseScrollPos() > 0)
			{
				g_cam.MoveForward(deltaTime * 100);
			}
		}

		if (g_input.MouseButtonDown(LMB))
		{
			g_cam.MoveUp(deltaTime);
		}

		if (g_input.MouseButtonDown(RMB))
		{
			g_cam.MoveDown(deltaTime);
		}

		if (g_input.MouseMoved())
		{
			g_cam.Pitch((g_input.GetMouseDiffY() * deltaTime));
			g_cam.Yaw((g_input.GetMouseDiffX() * deltaTime));
			
		}
	}
	XMVECTOR eyePosition = XMVectorSet(0, 0, -10, 1);
	XMVECTOR focusPoint = XMVectorSet(0, 0, 0, 1);
	XMVECTOR upDirection = XMVectorSet(0, 1, 0, 0);
	/*g_cam.SetPosition(0.0f, 0.0f, -10.0f);
	g_cam.LookAt(g_cam.GetPositionXM(), focusPoint, upDirection);
	g_cam.UpdateViewMatrix();*/
	g_cam.Update();
	g_viewMatrix = XMMatrixLookAtLH(eyePosition, focusPoint, upDirection);
	g_d3dDeviceContext->UpdateSubresource(g_d3dConstantBuffers[CB_Frame], 0, nullptr, &g_cam.GetCamData().viewMat, 0, 0);


	static float angle = 0.0f;
	angle += 90.0f * deltaTime;
	XMVECTOR rotationAxis = XMVectorSet(0, 1, 1, 0);
	g_worldMatrix = XMMatrixIdentity();
	//g_worldMatrix = XMMatrixRotationAxis(rotationAxis, XMConvertToRadians(angle));
	g_d3dDeviceContext->UpdateSubresource(g_d3dConstantBuffers[CB_Object], 0, nullptr, &g_worldMatrix, 0, 0);

	g_input.Reset();
}


// Clear the color and depth buffers
void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil)
{
	g_d3dDeviceContext->ClearRenderTargetView(g_d3dRenderTargetView, clearColor);
	g_d3dDeviceContext->ClearDepthStencilView(g_d3dDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, clearDepth, clearStencil);
}

void Present(bool vSync)
{
	if (vSync)
	{
		g_d3dSwapChain->Present(1, 0);
	}
	else
	{
		g_d3dSwapChain->Present(0, 0);
	}
}


void Render()
{
	assert(g_d3dDevice);
	assert(g_d3dDeviceContext);

	Clear(Colors::CornflowerBlue, 1.0f, 0);

	const UINT vertexStride = sizeof(VertexPosColor);
	const UINT offset = 0;

	g_d3dDeviceContext->IASetVertexBuffers(0, 1, &g_d3dVertexBuffer, &vertexStride, &offset);
	g_d3dDeviceContext->IASetInputLayout(g_d3dInputLayout);
	g_d3dDeviceContext->IASetIndexBuffer(g_d3dIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	g_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	g_d3dDeviceContext->VSSetShader(g_d3dVertexShader, nullptr, 0);
	g_d3dDeviceContext->VSSetConstantBuffers(0, 3, g_d3dConstantBuffers);

	g_d3dDeviceContext->RSSetState(g_d3dRasterizerState);
	g_d3dDeviceContext->RSSetViewports(1, &g_viewport);

	g_d3dDeviceContext->PSSetShader(g_d3dPixelShader, nullptr, 0);

	g_d3dDeviceContext->OMSetRenderTargets(1, &g_d3dRenderTargetView, g_d3dDepthStencilView);
	g_d3dDeviceContext->OMSetDepthStencilState(g_d3dDepthStencilState, 1);

	g_d3dDeviceContext->DrawIndexed(_countof(g_indicies), 0, 0);

	Present(g_enableVSync);
}

void Cleanup()
{

	g_d3dSwapChain->SetFullscreenState(FALSE, nullptr);
	SafeRelease(g_d3dDepthStencilView);
	SafeRelease(g_d3dRenderTargetView);
	SafeRelease(g_d3dDepthStencilBuffer);
	SafeRelease(g_d3dDepthStencilState);
	SafeRelease(g_d3dRasterizerState);
	SafeRelease(g_d3dSwapChain);
	SafeRelease(g_d3dDeviceContext);
	SafeRelease(g_d3dDevice);
}

void CaculateRenderStats()
{
	static int frameCount = 0;
	static float timeElapsed = 0.0f;

	frameCount++;

	if ((g_Timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCount;
		float frameRenderTime = 1000.0f / fps;


		std::ostringstream outs;
		outs.precision(6);
		outs << g_windowName << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << frameRenderTime << " (ms)";
		SetWindowText(g_windowHandle, outs.str().c_str());


		// Reset for next average.
		frameCount = 0;
		timeElapsed += 1.0f;

	}
}



