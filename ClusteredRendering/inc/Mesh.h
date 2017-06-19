#pragma once
#include "Vertex.h"
#include "Texture.h"

struct TexturePT
{
	unsigned int id;
	std::string type;
	aiString path;
};

struct Material
{
	Texture* diffuse;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex>* vertexList, std::vector<unsigned int>* indexList, std::vector<Texture*> textureList, ID3D11Device* device);
	~Mesh();
	
	void Render(ID3D11DeviceContext* context);

private:

	void InitBuffers(std::vector<Vertex>* vertexList, std::vector<unsigned int>* indexList, ID3D11Device* device);
	void InitTextures(std::vector<Texture> textureList, ID3D11Device* device, ID3D11DeviceContext* context);

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	unsigned int m_numVerts;
	unsigned int m_numIndices;
	unsigned int m_vertexOffset;
	unsigned int m_vertexStride;

	std::vector<Texture*> textures;



};