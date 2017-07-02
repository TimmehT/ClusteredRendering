#include <DirectXPCH.h>
#include "CTexture.h"
#include "WICTextureLoader.h"

CTexture::CTexture()
{
	m_textureSRV = nullptr;
}

CTexture::~CTexture()
{
	if (m_textureSRV)
	{
		SafeRelease(m_textureSRV);
	}
}

bool CTexture::LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* path)
{
	HRESULT hr;

	hr = CreateWICTextureFromFile(device, context, path, nullptr, &m_textureSRV);
	if (FAILED(hr))
	{
		return false;
	}

	return true;
}

void CTexture::PSSetSRV(ID3D11DeviceContext* context, unsigned int slot)
{
	context->PSSetShaderResources(slot, 1, &m_textureSRV);
}

const aiString CTexture::GetPath()
{
	return m_path;
}

void CTexture::SetPath(aiString &path)
{
	m_path = path;
}
