#pragma once
#include "Vert.h"
#include "CTexture.h"

class CMesh
{
public:
	CMesh(std::vector<Vert>* vertexList, std::vector<unsigned int>* indexList, std::vector<CTexture*> textureList, ID3D11Device* device);
	~CMesh();

	void Render(ID3D11DeviceContext* context);

private:

	void InitBuffers(std::vector<Vert>* vertexList, std::vector<unsigned int>* indexList, ID3D11Device* device);
	void InitTextures(std::vector<CTexture> textureList, ID3D11Device* device, ID3D11DeviceContext* context);

	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	unsigned int m_numVerts;
	unsigned int m_numIndices;
	unsigned int m_vertexOffset;
	unsigned int m_vertexStride;

	std::vector<CTexture*> textures;
};