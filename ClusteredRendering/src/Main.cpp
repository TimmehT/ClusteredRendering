#include <DirectXPCH.h>
#include "GameTimer.h"
#include "Camera.h"
#include "InputManager.h"
#include "Keycodes.h"
#include "WICTextureLoader.h"
#include "Model.h"
#include "CTexture.h"
#include "Shader.h"
#include "LightManager.h"
#include "ConstantVars.h"

// Define window & VSync Setting
unsigned __int16 g_windowWidth = Constants::CLIENT_WIDTH;
unsigned __int16 g_windowHeight = Constants::CLIENT_HEIGHT;
LPCSTR g_windowClassName = "DirectXWindowClass";
LPCSTR g_windowName = "Clustered Rendering Engine";
HWND g_windowHandle = 0;

const BOOL g_enableVSync = FALSE;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~DX11
// Direct 3D device and swap chain
ID3D11Device* g_d3dDevice = nullptr;
ID3D11DeviceContext* g_d3dDeviceContext = nullptr;
IDXGISwapChain* g_d3dSwapChain = nullptr;

// Render target view for the back buffer of the swap chain
ID3D11RenderTargetView* g_backBuffer = nullptr;
// Depth/stencil view for use as a depth buffer
ID3D11DepthStencilView* g_d3dDepthStencilView = nullptr;
// A texture to associate to the depth stencil view
ID3D11Texture2D* g_d3dDepthStencilBuffer = nullptr;

// Define the functionality of the depth/stencil stages
ID3D11DepthStencilState* g_d3dDepthStencilState = nullptr;
// Define the functionality of the rasterizer stage
ID3D11RasterizerState* g_d3dRasterizerState = nullptr;
D3D11_VIEWPORT g_viewport = { 0 };

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Shaders
Shader* g_vs;
Shader* g_ps;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~LightResources
LightManager g_lightManager;
ID3D11ShaderResourceView* depthBufferView;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ModelResources
Model g_sponza;

ID3D11Buffer* g_d3dObjectBuffer;

struct cbPerObject
{
	XMMATRIX g_worldMatrix;
	XMMATRIX g_invWorld;
	XMMATRIX g_wvp;
};

cbPerObject perObject;

ID3D11SamplerState* g_d3dSamplerState = nullptr;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~CameraResources
Camera g_cam;
XMMATRIX proj;
XMMATRIX view;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~PerFrameBuffers
ID3D11Buffer* g_d3dFrameBuffer;

struct cbPerFrame
{
	XMFLOAT3 eyePos;
	float pad;
};

cbPerFrame perFrame;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~Other
GameTimer g_Timer;
InputManager g_input;
float renderTime;
float prevRenderTime;
float mult = 1.0f;
bool animateLight = false;
std::vector<float>frameTimes;

// Forward declarations
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);


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

	hr = g_d3dDevice->CreateRenderTargetView(backBuffer, nullptr, &g_backBuffer);
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

	hr = g_d3dDevice->CreateShaderResourceView(g_d3dDepthStencilBuffer, nullptr, &depthBufferView);

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

	// Create constant buffer description
	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(D3D11_BUFFER_DESC));

	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(cbPerObject);
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	HRESULT hr = g_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dObjectBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	constantBufferDesc.ByteWidth = sizeof(cbPerFrame);

	hr = g_d3dDevice->CreateBuffer(&constantBufferDesc, nullptr, &g_d3dFrameBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	g_vs = new Shader(g_d3dDevice, g_d3dDeviceContext);
	g_vs->LoadShaderFromFile(VertexShader, L"../data/shaders/LightingVertexShader.hlsl", "main", "latest");

	g_ps = new Shader(g_d3dDevice, g_d3dDeviceContext);
	g_ps->LoadShaderFromFile(PixelShader, L"../data/shaders/LightingPixelShader.hlsl", "main", "latest");

	// Set up proj matrix
	g_cam.SetLens(Constants::FOV, Constants::NEARZ, Constants::FARZ, Constants::CLIENT_WIDTH, Constants::CLIENT_HEIGHT);
	proj = EngineMath::Float4X4ToMatrix(g_cam.GetCamData().projMat);

	// Create a sampler state for texture sampling in the pixel shader
	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));

	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	//samplerDesc.MaxAnisotropy = 16;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.BorderColor[0] = 1.0f;
	samplerDesc.BorderColor[1] = 1.0f;
	samplerDesc.BorderColor[2] = 1.0f;
	samplerDesc.BorderColor[3] = 1.0f;
	samplerDesc.MinLOD = -FLT_MAX;
	samplerDesc.MaxLOD = FLT_MAX;

	hr = g_d3dDevice->CreateSamplerState(&samplerDesc, &g_d3dSamplerState);
	if (FAILED(hr))
	{
		//MessageBoxA(m_Window.get_WindowHandle(), "Failed to create texture sampler.", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (!g_sponza.LoadModel("../data/models/crytek-sponza/sponza.obj", g_d3dDevice, g_d3dDeviceContext))
	{
		return false;
	}

	g_sponza.SetScale(0.01f, 0.01f, 0.01f);
	g_sponza.SetRotation(0.0f, 90.0f, 0.0f);
	g_sponza.SetTranslation(0.0f, -2.0f, 0.0f);
	g_sponza.SetWorldMatrix(g_sponza.GetModelData().m_scaleMatrix, g_sponza.GetModelData().m_rotationMatrix, g_sponza.GetModelData().m_translationMatrix);
	perObject.g_worldMatrix = XMMatrixIdentity();
	

	g_lightManager.InitBuffers(g_d3dDevice);


	g_d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return true;
}

void UnloadContent()
{
	SafeRelease(g_d3dSamplerState);
	SafeRelease(g_d3dFrameBuffer);
	SafeRelease(g_d3dObjectBuffer);
	SafeDelete(g_vs);
	SafeDelete(g_ps);

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
	CoInitialize(nullptr);
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
	CoUninitialize();

	

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
		
		if (g_input.KeyDown(VK_SHIFT))
		{
			mult = 2;
		}
		else
			mult = 1;

		if (g_input.KeyDown(VK_W))
		{
			g_cam.MoveForward(deltaTime * mult);
		}

		if (g_input.KeyDown(VK_S))
		{
			g_cam.MoveBackward(deltaTime* mult);
		}

		if (g_input.KeyDown(VK_A))
		{
			g_cam.MoveLeft(deltaTime* mult);
		}

		if (g_input.KeyDown(VK_D))
		{
			g_cam.MoveRight(deltaTime* mult);
		}

		if (g_input.KeyPressed(VK_L))
		{
			if (!animateLight)
				animateLight = true;
			else
				animateLight = false;
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
	g_cam.Update();
	
	if (animateLight)
	{
		g_lightManager.Update(deltaTime);
	}

	 view = EngineMath::Float4X4ToMatrix(g_cam.GetCamData().viewMat);

	 
	 g_input.Reset();
}


// Clear the color and depth buffers
void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil)
{
	g_d3dDeviceContext->ClearRenderTargetView(g_backBuffer, clearColor);
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

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	D3D11_MAPPED_SUBRESOURCE mappedResource2;

	Clear(Colors::CornflowerBlue, 1.0f, 0);

	g_vs->Push();

	perObject.g_worldMatrix = EngineMath::Float4X4ToMatrix(g_sponza.GetModelData().m_worldMatrix);
	perObject.g_wvp = perObject.g_worldMatrix * view * proj;
	perObject.g_wvp = XMMatrixTranspose(perObject.g_wvp);
	perObject.g_worldMatrix = XMMatrixTranspose(perObject.g_worldMatrix);
	perObject.g_invWorld = EngineMath::Float4X4ToMatrix(g_sponza.GetModelData().m_inverseWorld);

	g_d3dDeviceContext->Map(g_d3dObjectBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource2);
	memcpy(mappedResource2.pData, &perObject, sizeof(perObject));
	g_d3dDeviceContext->Unmap(g_d3dObjectBuffer, 0);
	g_d3dDeviceContext->VSSetConstantBuffers(0, 1, &g_d3dObjectBuffer);

	g_d3dDeviceContext->RSSetState(g_d3dRasterizerState);
	g_d3dDeviceContext->RSSetViewports(1, &g_viewport);

	g_ps->Push();

	perFrame.eyePos = g_cam.GetPosition();

	g_d3dDeviceContext->Map(g_d3dFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &perFrame, sizeof(perFrame));
	g_d3dDeviceContext->Unmap(g_d3dFrameBuffer, 0);
	g_d3dDeviceContext->PSSetConstantBuffers(0, 1, &g_d3dFrameBuffer);

	g_d3dDeviceContext->PSSetSamplers(0, 1, &g_d3dSamplerState);

	g_d3dDeviceContext->PSSetShaderResources(4, 1, &depthBufferView);
	g_lightManager.BuildClusters(g_cam.GetPosition(), g_cam.GetLook(), g_cam.GetRight(), g_cam.GetUp());
	g_lightManager.BindBuffer(g_d3dDeviceContext);
	g_d3dDeviceContext->OMSetRenderTargets(1, &g_backBuffer, g_d3dDepthStencilView);
	g_d3dDeviceContext->OMSetDepthStencilState(g_d3dDepthStencilState, 1);

	g_sponza.Render(g_d3dDeviceContext);

	Present(g_enableVSync);
}

void Cleanup()
{

	g_d3dSwapChain->SetFullscreenState(FALSE, nullptr);
	SafeRelease(depthBufferView);
	SafeRelease(g_d3dDepthStencilView);
	SafeRelease(g_backBuffer);
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
	float avg = 0;

	frameCount++;

	if ((g_Timer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCount;
		prevRenderTime = renderTime;
		renderTime = 1000.0f / fps;
		frameTimes.push_back(renderTime);
		float var = std::abs(renderTime - prevRenderTime);
		float totalframe = 0;
		for (unsigned int i = 0; i < frameTimes.size(); i++)
		{
			
			totalframe += frameTimes[i];
			avg = totalframe / frameTimes.size();

		}


		std::ostringstream outs;
		outs.precision(6);
		outs << g_windowName << "    "
			<< "FPS: " << fps << "    "
			<< "Frame Time: " << renderTime << " ms" << "    " 
			<< "Avg. Frame Time: " << avg << " ms" << "    "
			<< "var: +-" << var << " ms" ;
		SetWindowText(g_windowHandle, outs.str().c_str());


		// Reset for next average.
		frameCount = 0;
		timeElapsed += 1.0f;

	}
}



