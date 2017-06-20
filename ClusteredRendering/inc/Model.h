#pragma once
#include "CMesh.h"

class Model
{
public:

	Model();
	~Model();

	bool LoadModel(const char* file, ID3D11Device* device, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
private:

	void ProecessNode(aiNode* node, const aiScene* scene, ID3D11Device* device, ID3D11DeviceContext* context);
	CMesh* ProcessMesh(aiMesh* mesh, const aiScene* scene, ID3D11Device* device, ID3D11DeviceContext* context);
	std::vector<CTexture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, ID3D11Device* device, ID3D11DeviceContext* context);

	std::vector<CMesh*> m_meshList;
	std::vector<CTexture*> m_loadedTextures;
	std::string directory;
};