#pragma once
#include <malloc.h>

__declspec(align(16)) class Direct3DManager
{
public:
	Direct3DManager();
	Direct3DManager(const Direct3DManager&);
	~Direct3DManager();

	void* operator new(size_t i)
	{
		return _mm_malloc(i, 16);
	}

	void operator delete(void* p)
	{
		_mm_free(p);
	}

	bool Initialize(bool, HWND, bool, float, float);
	void Unitialize();

	void Clear(const FLOAT[4], FLOAT, UINT8);
	void SetRasterizerStage();
	void SetOutputMergerStage();
	void Present(bool);

	DXGI_RATIONAL QueryRefreshRate(UINT,UINT, BOOL);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();

	void GetProjectionMatrix(XMMATRIX&);
	void GetWorldMatrix(XMMATRIX&);
	void GetOrthoMatrix(XMMATRIX&);

private:
	bool m_isVSyncEnabled;
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11RasterizerState* m_rasterizerState;
	D3D11_VIEWPORT m_viewport;
	XMMATRIX m_projectionMatrix;
	XMMATRIX m_worldMatrix;
	XMMATRIX m_orthoMatrix;

};