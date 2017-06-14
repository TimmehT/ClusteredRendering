#pragma once
#include "Vertex.h"

struct Texture
{
	unsigned int id;
	std::string type;
};

class Mesh
{
public:
	Mesh(std::vector<Vertex> vertexList, std::vector<unsigned int> indexList, std::vector<Texture> textureList);
	~Mesh();
	
	void Render();

	std::vector<Vertex> m_vertexList;
	std::vector<unsigned int> m_indexList;
	std::vector<Texture> m_textureList;

private:

	void InitBuffers();

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	unsigned int m_numVerts;
	unsigned int m_numIndices;
	unsigned int m_vertexOffset;
	unsigned int m_vertexStride;



};