#pragma once
#include<DirectXPCH.h>

struct Vert
{
	Vert() {};
	Vert(XMFLOAT3 pos, XMFLOAT3 norm, XMFLOAT2 texc)
	{
		position = pos;
		normal = norm;
		texcoord = texc;
	}
	
	XMFLOAT3 position;
	XMFLOAT3 normal;
	XMFLOAT2 texcoord;
	XMFLOAT3 tangent;
	XMFLOAT3 bitangent;
};