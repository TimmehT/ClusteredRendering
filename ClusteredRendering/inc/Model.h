#pragma once
#include "Mesh.h"

class Model
{
public:

	Model();
	~Model();

	void LoadModel(const char* file);
	void Render();
private:

	void ProecessNode(aiNode* node, const aiScene* scene);
	Mesh ProcessMesh(aiMesh* mesh, const aiScene* scene);

	std::vector<Mesh> m_meshList;
};