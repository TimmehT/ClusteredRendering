#include <DirectXPCH.h>
#include "CMaterial.h"

CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
}

void CMaterial::SetColor(ColorType type, XMFLOAT4 color)
{
}

void CMaterial::SetTexture(TextureType type, CTexture* texture)
{
	m_textures[type] = texture;

	switch (type)
	{
	case TextureType::Ambient:
	{
		
	}
	break;
	case TextureType::Diffuse:
	{

	}
	break;
	case TextureType::Specular:
	{

	}
	break;
	case TextureType::Emissive:
	{

	}
	break;
	case TextureType::Normal:
	{

	}
	break;
	case TextureType::Opacity:
	{

	}
	break;
	}
}

void CMaterial::SetAlphaThreshold(float thresh)
{
}
