#include "DirectXPCH.h"
#include "RenderTarget.h"

//extern ID3D11Device* device;
//extern ID3D11DeviceContext* context;
//
//RenderTarget::RenderTarget()
//{
//	m_renderTargetView = nullptr;
//	m_renderSRV = nullptr;
//}
//
//RenderTarget::~RenderTarget()
//{
//	SafeRelease(m_renderTargetView);
//	SafeRelease(m_renderSRV);
//}
//
//void RenderTarget::CreateRenderTarget(unsigned width, unsigned height, DXGI_FORMAT format)
//{
//	HRESULT hr;
//
//	D3D11_TEXTURE2D_DESC desc;
//	ZeroMemory(&desc, sizeof(desc));
//	desc.Width = width;
//	desc.Height = height;
//	desc.MipLevels = 1;
//	desc.ArraySize = 1;
//	desc.Format = format;
//	desc.SampleDesc.Count = 1;
//	desc.Usage = D3D11_USAGE_DEFAULT;
//	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
//
//	D3D11_RENDER_TARGET_VIEW_DESC rtdesc;
//	ZeroMemory(&rtdesc, sizeof(rtdesc));
//	rtdesc.Format = format;
//	rtdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
//
//	ID3D11Texture2D* testTex;
//	hr = device->CreateTexture2D(&desc, nullptr, &testTex);
//
//
//	SafeRelease(testTex);
//
//
//
//}
//
//ID3D11RenderTargetView* RenderTarget::GetRenderTargetView()
//{
//	return m_renderTargetView;
//}
//
//void RenderTarget::PSSetSRV(unsigned slot)
//{
//	context->PSSetShaderResources(slot, 1, &m_renderSRV);
//}
//
//void RenderTarget::Clear()
//{
//	float clearColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
//	context->ClearRenderTargetView(m_renderTargetView, clearColor);
//}



