#include <DirectXPCH.h>
#include "CMesh.h"


CMesh::CMesh(std::vector<Vert>* vertexList, std::vector<unsigned int>* indexList, CMaterial* mat, ID3D11Device* device)
{
	m_indexBuffer = nullptr;
	m_vertexBuffer = nullptr;
	material = mat;
	m_numVerts = vertexList->size();
	m_numIndices = indexList->size();
	m_vertexStride = sizeof(Vert);
	m_vertexOffset = 0;
	//textures = textureList;

	InitBuffers(vertexList, indexList, device);
}

CMesh::~CMesh()
{
	if (m_indexBuffer)
	{
		SafeRelease(m_indexBuffer);
	}

	if (m_vertexBuffer)
	{
		SafeRelease(m_vertexBuffer);
	}

	SafeDelete(material);

}

void CMesh::Render(ID3D11DeviceContext* context)
{
	context->IASetVertexBuffers(0, 1, &m_vertexBuffer, &m_vertexStride, &m_vertexOffset);
	context->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//if (textures.size() != 0)
	//{
		//textures[0]->PSSetSRV(context, 0);
	//}

	if (material != nullptr)
	{
		material->Bind(context);
	}
	

	context->DrawIndexed(m_numIndices, 0, 0);

}

void CMesh::InitBuffers(std::vector<Vert>* vertexList, std::vector<unsigned int>* indexList, ID3D11Device* device)
{

	// Create an initialize the vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = m_vertexStride * m_numVerts;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	resourceData.pSysMem = &vertexList->at(0);

	HRESULT hr = device->CreateBuffer(&vertexBufferDesc, &resourceData, &m_vertexBuffer);
	if (FAILED(hr))
	{
		//return false;
	}

	// Create and initialize the index buffer.
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * m_numIndices;
	indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	resourceData.pSysMem = &indexList->at(0);

	hr = device->CreateBuffer(&indexBufferDesc, &resourceData, &m_indexBuffer);
	if (FAILED(hr))
	{
		//return false;
	}
}

void CMesh::InitTextures(std::vector<CTexture> textureList, ID3D11Device * device, ID3D11DeviceContext * context)
{
	for (unsigned int i = 0; i < textureList.size(); i++)
	{
		//textureList[i].LoadTextureFromFile(device, context, );
	}
}
