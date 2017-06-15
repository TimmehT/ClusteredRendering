#include "DirectXPCH.h"
#include "Model.h"
#include "Texture.h"

Model::Model()
{
}

Model::~Model()
{
	for (unsigned int i = 0; i < m_meshList.size(); i++)
	{
		SafeDelete(m_meshList[i]);
	}
}

bool Model::LoadModel(const char* file, ID3D11Device* device)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		return false;
	}


	directory = file;
	directory = directory.substr(0, directory.find_last_of('/'));

	ProecessNode(scene->mRootNode, scene, device);

	return true;
}

void Model::Render(ID3D11DeviceContext* context)
{
	for (unsigned int i = 0; i < m_meshList.size(); i++)
	{
		m_meshList[i]->Render(context);
	}
}

void Model::ProecessNode(aiNode * node, const aiScene* scene, ID3D11Device* device)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshList.push_back(ProcessMesh(mesh, scene, device));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProecessNode(node->mChildren[i], scene, device);
	}
}

Mesh* Model::ProcessMesh(aiMesh * mesh, const aiScene * scene, ID3D11Device* device)
{
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.position = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		if (mesh->mTextureCoords[0])
		{
			vertex.texcoord = XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
			vertex.texcoord = XMFLOAT2(0.0f, 0.0f);

		vertices.push_back(vertex);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		for (unsigned int j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		/*std::vector<TexturePT> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<TexturePT> normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());*/
	}
	return new Mesh(&vertices,&indices,textures, device);
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName)
{

	std::vector<Texture> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (unsigned int j = 0; j < m_loadedTextures.size(); j++)
		{
			if (std::strcmp(m_loadedTextures[j].GetTexData().path.C_Str(), str.C_Str()) == 0)
			{
				textures.push_back(m_loadedTextures[j]);
				skip = true;
				break;
			}
			
		}

		if (!skip)
		{
			Texture texture;

			std::string filepath = std::string(str.data);
			std::replace(filepath.begin(), filepath.end(), '\\', '/');
			filepath = std::string(directory.c_str()) + '/' + std::string(filepath.c_str());
			wchar_t wc_path[MAX_PATH];

			mbstowcs_s(nullptr, wc_path, filepath.c_str(), MAX_PATH);

			

			texture.SetPath(str);
			textures.push_back(texture);
			m_loadedTextures.push_back(texture);
		}

	}
	return textures;
}
