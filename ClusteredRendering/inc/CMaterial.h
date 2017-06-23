#pragma once
#include "CTexture.h"

struct MaterialProperties
{
	MaterialProperties()
		: m_globalAmbient(0.1f, 0.1f, 0.1f, 1.0f)
		, m_ambientColor(0, 0, 0, 1)
		, m_diffuseColor(1, 1, 1, 1)
		, m_specularColor(0, 0, 0, 1)
		, m_emissiveColor(0, 0, 0, 1)
		, m_useDiffuseTexture(false)
		, m_useSpecularTexture(false)
		, m_useEmmisiveTexture(false)
		, m_useNormalTexture(false)
		, m_useOpacityTexture(false)
		, m_alphaThreshold(0.1f)

	{
		ZeroMemory(this, sizeof(this));
	}

	XMFLOAT4 m_globalAmbient;
	//-------------------------(16b)
	XMFLOAT4 m_ambientColor;
	//-------------------------(16b)
	XMFLOAT4 m_diffuseColor;
	//-------------------------(16b)
	XMFLOAT4 m_specularColor;
	//-------------------------(16b)
	XMFLOAT4 m_emissiveColor;
	//-------------------------(16b)
	uint32_t m_useDiffuseTexture;
	uint32_t m_useSpecularTexture;
	uint32_t m_useEmmisiveTexture;
	uint32_t m_useNormalTexture;
	//-------------------------(16b)
	uint32_t m_useOpacityTexture;
	float m_alphaThreshold;
	XMFLOAT2 m_pad;
	//-------------------------(16b)
	// 7 * 16 = 112 bytes
};

enum class TextureType : uint32_t
{
	Ambient = 0,
	Diffuse = 1,
	Specular = 2,
	Emissive = 3,
	Normal = 4,
	Opacity = 5
};

enum ColorType
{
	GlobalAmbient = 0,
	Ambient = 1,
	Diffuse = 2,
	Specular = 3,
	Emissive = 4
};

class CMaterial
{
public:

	CMaterial();
	~CMaterial();


	void SetColor(ColorType type, XMFLOAT4 color);
	void SetTexture(TextureType type, CTexture* texture);
	void SetAlphaThreshold(float thresh);

private:

	typedef std::map<TextureType, CTexture*> TextureMap;
	TextureMap m_textures;

};