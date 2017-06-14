#include "DirectXPCH.h"
#include "Mesh.h"

extern ID3D11Device* g_d3dDevice;
extern ID3D11DeviceContext* g_d3dDeviceContext;

Mesh::Mesh(std::vector<Vertex> vertexList, std::vector<unsigned int> indexList, std::vector<Texture> textureList)
{

	m_vertexList = vertexList;
	m_indexList = indexList;
	m_numVerts = vertexList.size();
	m_numIndices = indexList.size();
	m_vertexStride = sizeof(Vertex);
	m_vertexOffset = 0;

	InitBuffers();
}

Mesh::~Mesh()
{
	//SafeRelease(m_indexBuffer);
	//SafeRelease(m_vertexBuffer);
}

void Mesh::Render()
{
	g_d3dDeviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &m_vertexStride, &m_vertexOffset);
	g_d3dDeviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	g_d3dDeviceContext->DrawIndexed(m_numIndices, 0, 0);
	
}

void Mesh::InitBuffers()
{

	// Create an initialize the vertex buffer.
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));

	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.ByteWidth = m_vertexStride * m_numVerts;
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA resourceData;
	ZeroMemory(&resourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	resourceData.pSysMem = &m_vertexList.at(0);

	HRESULT hr = g_d3dDevice->CreateBuffer(&vertexBufferDesc, &resourceData, &m_vertexBuffer);
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

	resourceData.pSysMem = &m_indexList.at(0);

	hr = g_d3dDevice->CreateBuffer(&indexBufferDesc, &resourceData, &m_indexBuffer);
	if (FAILED(hr))
	{
		//return false;
	}
}
