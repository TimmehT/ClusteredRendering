#include <DirectXPCH.h>
#include "LightManager.h"


static float GetRandomFloat(float min, float max)
{
	return (float)rand() / (RAND_MAX + 1) * (max - min) + min;
}

static XMFLOAT4 GetRandomColor()
{
	static unsigned counter = 0;
	counter++;

	XMFLOAT4 color;
	if (counter % 2 == 0)
	{
		color = XMFLOAT4(GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.9f, 1.0f), GetRandomFloat(0.0f, 1.0f), 1.0f);
	}
	else
	{
		color = XMFLOAT4(GetRandomFloat(0.9f, 1.0f), GetRandomFloat(0.0f, 1.0f), GetRandomFloat(0.0f, 1.0f), 1.0f);
	}

	return color;
}


LightManager::LightManager()
{
	
}

LightManager::~LightManager()
{

	SafeRelease(m_lightSRV);
	SafeRelease(m_lightBuffer);
}

void LightManager::InitBuffers(ID3D11Device* device)
{

	D3D11_BUFFER_DESC lightBufferDesc;
	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	lightBufferDesc.ByteWidth = sizeof(Light) * Constants::NUM_LIGHTS;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	lightBufferDesc.StructureByteStride = sizeof(Light);

	RandomlyDistributeLights();

	D3D11_SUBRESOURCE_DATA lightInitData;
	lightInitData.pSysMem = &m_lights[0];
	lightInitData.SysMemPitch = 0;
	lightInitData.SysMemSlicePitch = 0;

	device->CreateBuffer(&lightBufferDesc, &lightInitData, &m_lightBuffer);
	

	D3D11_SHADER_RESOURCE_VIEW_DESC lightSRVDesc;
	ZeroMemory(&lightSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	lightSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	lightSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	lightSRVDesc.BufferEx.FirstElement = 0;
	lightSRVDesc.BufferEx.NumElements = Constants::NUM_LIGHTS;

	device->CreateShaderResourceView(m_lightBuffer, &lightSRVDesc, &m_lightSRV);
	
}

void LightManager::BindBuffer(ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE lightResource;

	context->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &lightResource);
	memcpy(lightResource.pData, &m_lights[0], Constants::NUM_LIGHTS * sizeof(Light));
	context->Unmap(m_lightBuffer, 0);

	context->PSSetShaderResources(5, 1, &m_lightSRV);
}

void LightManager::Update(float dt)
{
	float rotation = dt * DirectX::XM_PIDIV2;

	XMMATRIX rot = XMMatrixRotationY(rotation);
	for (int i = 0; i < Constants::NUM_LIGHTS; i++)
	{
		XMStoreFloat4(&m_lights[i].m_positionWS, XMVector4Transform(XMLoadFloat4(&m_lights[i].m_positionWS), rot));
	}
}

void LightManager::RandomlyDistributeLights()
{
	XMFLOAT3 max = XMFLOAT3(17.9990796f, 14.2943311f, 11.8280701f);
	XMFLOAT3 min = XMFLOAT3(-19.2094580f, -12.6442505f, -11.0542590f);

	float range = 10.0f;

	for (int i = 0; i < Constants::NUM_LIGHTS; i++)
	{
		m_lights[i].m_positionWS = XMFLOAT4(GetRandomFloat(min.x, max.x), GetRandomFloat(min.y, max.z), GetRandomFloat(min.z, max.z), 1.0f);
		m_lights[i].m_directionWS = XMFLOAT4(0, -1, 0, 0);
		m_lights[i].m_color = GetRandomColor();
		m_lights[i].m_range = 10.0f;
		m_lights[i].m_spotAngle = 45.0f;
		m_lights[i].m_enabled = true;
		m_lights[i].m_type = LightType::Point;
	}

}
