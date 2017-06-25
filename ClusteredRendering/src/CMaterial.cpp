#include <DirectXPCH.h>
#include "CMaterial.h"


CMaterial::CMaterial(ID3D11Device* device)
	: m_update(false)
{
	InitBuffer(device);
}

CMaterial::~CMaterial()
{
	SafeRelease(m_matConstantBuffer);
}

void CMaterial::SetColor(ColorType type, XMFLOAT4 color)
{
	switch (type)
	{
	case ColorType::GlobalAmbient:
	{
		m_matProperties.m_globalAmbient = color;
	}
	break;
	case ColorType::Ambient:
	{
		m_matProperties.m_ambientColor = color;
	}
	break;
	case ColorType::Diffuse:
	{
		m_matProperties.m_diffuseColor = color;
	}
	break;
	case ColorType::Specular:
	{
		m_matProperties.m_specularColor = color;
	}
	break;
	}

	m_update = true;
}

void CMaterial::SetTexture(TextureType type, CTexture* texture)
{
	m_textures[type] = texture;

	switch (type)
	{
	case TextureType::Diffuse:
	{
		m_matProperties.m_useDiffuseTexture = (texture != nullptr);
	}
	break;
	case TextureType::Specular:
	{
		m_matProperties.m_useSpecularTexture = (texture != nullptr);
	}
	break;
	case TextureType::Normal:
	{
		m_matProperties.m_useNormalTexture = (texture != nullptr);
	}
	break;
	case TextureType::Opacity:
	{
		m_matProperties.m_useOpacityTexture = (texture != nullptr);
	}
	break;
	}

	m_update = true;
}

void CMaterial::SetAlphaThreshold(float thresh)
{
	m_matProperties.m_alphaThreshold = thresh;
	m_update = true;
}

void CMaterial::InitBuffer(ID3D11Device * device)
{
	D3D11_BUFFER_DESC matBufferDesc;
	ZeroMemory(&matBufferDesc, sizeof(D3D11_BUFFER_DESC));

	matBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matBufferDesc.ByteWidth = sizeof(MaterialProperties);
	matBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matBufferDesc.Usage = D3D11_USAGE_DYNAMIC;

	HRESULT hr = device->CreateBuffer(&matBufferDesc, nullptr, &m_matConstantBuffer);
	if (FAILED(hr))
	{
		//Error
		return;
	}
}

void CMaterial::UpdateBuffer(ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	if (FAILED(context->Map(m_matConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
	{
		//Error
		return;
	}

	memcpy(mappedResource.pData, &m_matProperties, sizeof(m_matProperties));

	context->Unmap(m_matConstantBuffer, 0);
}

void CMaterial::Bind(ID3D11DeviceContext* context)
{
	if (m_update)
	{
		UpdateBuffer(context);
		m_update = false;
	}


		for (auto texture : m_textures)
		{
			if (texture.second != nullptr)
			{
				//CTexture* tex = texture.second;
				texture.second->PSSetSRV(context, (uint32_t)texture.first);
			}

		}

		context->PSSetConstantBuffers(1, 1, &m_matConstantBuffer);
	
}
