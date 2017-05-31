#pragma once
#include <DirectXPCH.h>


struct Vertex
{
	Vertex() {};
	Vertex(XMFLOAT3 pos, XMFLOAT3 norm, XMFLOAT2 texc)
	{
		position = pos;
		normal = norm;
		texcoord = texc;
	}

	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texcoord;
};

struct VertexPT
{
	VertexPT() {};
	VertexPT(XMFLOAT3 pos)
	{
		position = pos;
	}

	XMFLOAT3 position;
};
