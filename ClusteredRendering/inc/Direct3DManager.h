#pragma once

class Direct3DManager
{
public:
	Direct3DManager();
	Direct3DManager(const Direct3DManager&);
	~Direct3DManager();

	bool Initialize(HWND hwnd, bool vSync);
	void Cleanup();

	void Clear(const FLOAT clearColor[4], FLOAT clearDepth, UINT8 clearStencil);
	void Present(bool vSync);
	void OnResize(unsigned __int16 clientWidth, unsigned __int16 clientHeight);

	DXGI_RATIONAL QueryRefreshRate(UINT clientWidth, UINT clientHeight, BOOL vSync);

	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetDeviceContext();


private:

	bool ResizeSwapChain( unsigned __int16 clientWidth, unsigned __int16 clientHeight);

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

};
