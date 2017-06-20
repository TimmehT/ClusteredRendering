#pragma once

struct TextureData
{
	aiString path;
	wchar_t finalPath;
	std::string type;
};

class CTexture
{
public:
	CTexture();
	~CTexture();

	bool LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* path);
	void PSSetSRV(ID3D11DeviceContext* context, unsigned int slot);

	const TextureData& GetTexData();

	void SetPath(aiString &path);
	void SetFinalPath(const wchar_t* path);

private:
	ID3D11ShaderResourceView* m_textureSRV;
	aiString path;
	std::string m_type;

	TextureData m_texData;

};