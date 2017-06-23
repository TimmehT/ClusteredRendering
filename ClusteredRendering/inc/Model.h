#pragma once
#include "CMesh.h"

struct ModelData
{
	XMFLOAT4X4 m_worldMatrix;
	XMFLOAT4X4 m_inverseWorld;
	XMFLOAT4X4 m_translationMatrix;
	XMFLOAT4X4 m_scaleMatrix;
	XMFLOAT4X4 m_rotationMatrix;
};

class Model
{
public:

	Model();
	~Model();

	bool LoadModel(const char* file, ID3D11Device* device, ID3D11DeviceContext* context);
	void Render(ID3D11DeviceContext* context);
	void SetScale(float x, float y, float z);
	void SetRotation(float x, float y, float z);
	void SetTranslation(float x, float y, float z);
	void SetWorldMatrix(XMFLOAT4X4 x, XMFLOAT4X4 y, XMFLOAT4X4 z);

	ModelData& GetModelData();

private:

	void ProecessNode(aiNode* node, const aiScene* scene, ID3D11Device* device, ID3D11DeviceContext* context);
	CMesh* ProcessMesh(aiMesh* mesh, const aiScene* scene, ID3D11Device* device, ID3D11DeviceContext* context);
	std::vector<CTexture*> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, ID3D11Device* device, ID3D11DeviceContext* context);
	void LoadMaterialProperties(aiMaterial* mat);

	std::vector<CMesh*> m_meshList;
	std::vector<CTexture*> m_loadedTextures;
	std::string directory;

	ModelData m_modelData;

	
};