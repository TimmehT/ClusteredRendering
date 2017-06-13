#pragma once
#include <DirectXPCH.h>
#include "Vertex.h"

struct Texture
{
	unsigned int id;
	std::string type;
};

class Mesh
{
public:
	std::vector<Vertex> vertexList;
	std::vector<unsigned int> indexList;
	std::vector<Texture> textureList;

	Mesh(std::vector<Vertex> vertexList, std::vector<unsigned int> indexList, std::vector<Texture> textureList);
};