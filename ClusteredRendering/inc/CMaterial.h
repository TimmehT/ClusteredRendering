#pragma once
#include "CTexture.h"


class CMaterial
{
public:

	enum class TextureType : uint32_t
	{
		Diffuse = 0,
		Specular = 1,
		Normal = 2,
		Opacity = 3
	};

	enum ColorType
	{
		GlobalAmbient = 0,
		Ambient = 1,
		Diffuse = 2,
		Specular = 3,
	};

	CMaterial(ID3D11Device* device);
	~CMaterial();


	void SetColor(ColorType type, XMFLOAT4 color);
	void SetSpecularPower(float pow);
	void SetTexture(TextureType type, CTexture* texture);
	void SetAlphaThreshold(float thresh);
	void InitBuffer(ID3D11Device* device);
	void UpdateBuffer(ID3D11DeviceContext* context);
	void Bind(ID3D11DeviceContext* context);

private:

	struct MaterialProperties
	{
		MaterialProperties()
			: m_globalAmbient(0.2f, 0.2f, 0.2f, 1.0f)
			, m_ambientColor(0, 0, 0, 1)
			, m_diffuseColor(1, 1, 1, 1)
			, m_specularColor(0, 0, 0, 1)
			, m_useDiffuseTexture(false)
			, m_useSpecularTexture(false)
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
		uint32_t m_useDiffuseTexture;
		uint32_t m_useSpecularTexture;
		uint32_t m_useNormalTexture;
		uint32_t m_useOpacityTexture;
		//-------------------------(16b)
		float m_alphaThreshold;
		XMFLOAT3 m_pad;
		//-------------------------(16b)
		// 
	};

	typedef std::map<TextureType, CTexture*> TextureMap;
	TextureMap m_textures;

	MaterialProperties m_matProperties;

	ID3D11Buffer* m_matConstantBuffer;

	bool m_update;

};