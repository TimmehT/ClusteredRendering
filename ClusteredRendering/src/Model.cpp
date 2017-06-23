#include <DirectXPCH.h>
#include "Model.h"



Model::Model()
{
}

Model::~Model()
{
	for (unsigned int i = 0; i < m_meshList.size(); i++)
	{
		SafeDelete(m_meshList[i]);
	}

	for (unsigned int j = 0; j < m_loadedTextures.size(); j++)
	{
		SafeDelete(m_loadedTextures[j]);
	}
}

bool Model::LoadModel(const char* file, ID3D11Device* device, ID3D11DeviceContext* context)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(file, aiProcess_Triangulate | aiProcess_ConvertToLeftHanded | aiProcess_CalcTangentSpace);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
	{
		return false;
	}


	directory = file;
	directory = directory.substr(0, directory.find_last_of('/'));

	ProecessNode(scene->mRootNode, scene, device, context);

	return true;
}

void Model::Render(ID3D11DeviceContext* context)
{
	for (unsigned int i = 0; i < m_meshList.size(); i++)
	{
		m_meshList[i]->Render(context);
	}
}

void Model::SetScale(float x, float y, float z)
{
	XMMATRIX scale = XMMatrixScaling(x, y, z);
	XMStoreFloat4x4(&m_modelData.m_scaleMatrix, scale);
}

void Model::SetRotation(float x, float y, float z)
{
	XMMATRIX rot = XMMatrixRotationX(XMConvertToRadians(x));
	rot *= XMMatrixRotationY(XMConvertToRadians(y));
	rot *= XMMatrixRotationZ(XMConvertToRadians(z));
	XMStoreFloat4x4(&m_modelData.m_rotationMatrix, rot);
}

void Model::SetTranslation(float x, float y, float z)
{
	XMMATRIX trans = XMMatrixTranslation(x, y, z);
	XMStoreFloat4x4(&m_modelData.m_translationMatrix, trans);
}

void Model::SetWorldMatrix(XMFLOAT4X4 x, XMFLOAT4X4 y, XMFLOAT4X4 z)
{
	XMMATRIX xM = EngineMath::Float4X4ToMatrix(x);
	XMMATRIX yM = EngineMath::Float4X4ToMatrix(y);
	XMMATRIX zM = EngineMath::Float4X4ToMatrix(z);
	XMMATRIX world = XMMatrixIdentity();
	world = xM * yM * zM;
	XMStoreFloat4x4(&m_modelData.m_worldMatrix, world);
	XMStoreFloat4x4(&m_modelData.m_inverseWorld, XMMatrixInverse(nullptr,world));
}

ModelData & Model::GetModelData()
{
	return m_modelData;
}

void Model::ProecessNode(aiNode * node, const aiScene* scene, ID3D11Device* device, ID3D11DeviceContext* context)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		m_meshList.push_back(ProcessMesh(mesh, scene, device, context));
	}

	for (unsigned int i = 0; i < node->mNumChildren; i++)
	{
		ProecessNode(node->mChildren[i], scene, device, context);
	}
}

CMesh* Model::ProcessMesh(aiMesh * mesh, const aiScene * scene, ID3D11Device* device, ID3D11DeviceContext* context)
{
	std::vector<Vert> vertices;
	std::vector<unsigned int> indices;
	std::vector<CTexture*> textures;
	

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		Vert vertex;
		vertex.position = XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
		vertex.normal = XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);

		if (mesh->mTextureCoords[0])
		{
			vertex.texcoord = XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
		}
		else
			vertex.texcoord = XMFLOAT2(0.0f, 0.0f);

		vertex.tangent = XMFLOAT3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
		vertex.bitangent = XMFLOAT3(mesh->mBitangents[i].x, mesh->mBitangents[i].y, mesh->mBitangents[i].z);

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

		std::vector<CTexture*> diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", device, context);
		textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());

		std::vector<CTexture*> specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", device, context);
		textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());

		std::vector<CTexture*> normalMaps = LoadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", device, context);
		textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());

	}
	return new CMesh(&vertices,&indices,textures, device);
}

std::vector<CTexture*> Model::LoadMaterialTextures(aiMaterial * mat, aiTextureType type, std::string typeName, ID3D11Device* device, ID3D11DeviceContext* context)
{

	std::vector<CTexture*> textures;

	for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
	{
		aiString str;
		mat->GetTexture(type, i, &str);

		bool skip = false;
		for (unsigned int j = 0; j < m_loadedTextures.size(); j++)
		{
			if (std::strcmp(m_loadedTextures[j]->GetTexData().path.C_Str(), str.C_Str()) == 0)
			{
				textures.push_back(m_loadedTextures[j]);
				skip = true;
				break;
			}
			
		}

		if (!skip)
		{
			CTexture* texture = new CTexture();

			std::string filepath = std::string(str.data);

			if (filepath.rfind('\\') != 0)
			{
				std::replace(filepath.begin(), filepath.end(), '\\', '/');
			}
			
			filepath = std::string(directory.c_str()) + '/' + std::string(filepath.c_str());
			wchar_t wc_path[MAX_PATH];
			wchar_t wc_p;


			mbstowcs_s(nullptr, wc_path, filepath.c_str(), MAX_PATH);

			texture->LoadTextureFromFile(device, context, wc_path);
			
			//texture.SetFinalPath(wc_path);
			texture->SetPath(str);
			textures.push_back(texture);
			m_loadedTextures.push_back(texture);
		}

	}
	return textures;
}
