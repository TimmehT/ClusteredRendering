#pragma once
#include "Mesh.h"

class Model
{
public:

	Model();
	~Model();

	bool LoadModel(const char* file, ID3D11Device* device, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
private:

	void ProecessNode(aiNode* node, const aiScene* scene, ID3D11Device* device, ID3D11DeviceContext* context);
	Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene, ID3D11Device* device, ID3D11DeviceContext* context);
	std::vector<Texture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, ID3D11Device* device, ID3D11DeviceContext* context);

	std::vector<Mesh*> m_meshList;
	std::vector<Texture*> m_loadedTextures;
	std::string directory;

	
};