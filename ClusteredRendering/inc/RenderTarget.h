#pragma once


class RenderTarget
{
public:
	RenderTarget();
	~RenderTarget();

	void CreateRT(unsigned width, unsigned height, DXGI_FORMAT);
	ID3D11RenderTargetView* GetRTV();
	void PSSetSRV();
	void Clear();

private:
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11ShaderResourceView* m_renderTargetSRV;
};