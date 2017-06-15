#include <DirectXPCH.h>
#include "Texture.h"
#include "WICTextureLoader.h"

Texture::Texture()
{
	m_textureSRV = nullptr;
}

Texture::~Texture()
{
	if (m_textureSRV)
	{
		SafeRelease(m_textureSRV);
	}
}

bool Texture::LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* path)
{
	HRESULT hr;

	hr = CreateWICTextureFromFile(device, context, path, nullptr, &m_textureSRV);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void Texture::PSSetSRV(ID3D11DeviceContext* context, unsigned int slot)
{
	context->PSSetShaderResources(slot, 1, &m_textureSRV);
}