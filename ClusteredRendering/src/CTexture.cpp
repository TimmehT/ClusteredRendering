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

const TextureData & CTexture::GetTexData()
{
	// TODO: insert return statement here
	return m_texData;
}

void CTexture::SetPath(aiString &path)
{
	m_texData.path = path;
}

void CTexture::SetFinalPath(const wchar_t* path)
{
	wcscpy_s(&m_texData.finalPath, MAX_PATH, path);
}
