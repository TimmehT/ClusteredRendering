#pragma once
#include "Mesh.h"

class Model
{
public:

	Model();
	~Model();

	bool LoadModel(const char* file, ID3D11Device* device);
	void Render(ID3D11DeviceContext* context);
private:

	void ProecessNode(aiNode* node, const aiScene* scene, ID3D11Device* device);
	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene, ID3D11Device* device);
	std::vector<Texture> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);

	std::vector<Mesh*> m_meshList;
	std::vector<Texture> m_loadedTextures;
	std::string directory;

	
};