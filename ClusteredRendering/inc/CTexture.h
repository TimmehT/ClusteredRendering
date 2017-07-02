#pragma once

class CTexture
{
public:
	CTexture();
	~CTexture();

	bool LoadTextureFromFile(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* path);
	void PSSetSRV(ID3D11DeviceContext* context, unsigned int slot);

	const aiString GetPath();

	void SetPath(aiString &path);

private:
	ID3D11ShaderResourceView* m_textureSRV;
	aiString m_path;

};