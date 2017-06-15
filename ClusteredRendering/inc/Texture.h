#pragma once

struct TextureData
{
	ID3D11ShaderResourceView* textureSRV;
	aiString path;
	std::string type;
};

class Texture
{
public:
	Texture();
	~Texture();

	bool LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* path);
	void PSSetSRV(ID3D11DeviceContext* context, unsigned int slot);

private:
	ID3D11ShaderResourceView* m_textureSRV;
	aiString path;
	std::string m_type;

};