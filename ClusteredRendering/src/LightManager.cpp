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
	m_clusterLightList = std::vector<short>(Constants::LIGHT_INDEX_COUNT);
	m_lightIndexList= std::vector<int>(Constants::LIGHT_INDEX_COUNT);
}

LightManager::~LightManager()
{
	SafeRelease(m_clusterIndexOffsetView);
	SafeRelease(m_clusterOffsetTex);
	SafeRelease(m_lightIndexBufferView);
	SafeRelease(m_lightIndexBuffer);
	SafeRelease(m_lightSRV);
	SafeRelease(m_lightBuffer);
}

void LightManager::InitBuffers(ID3D11Device* device)
{

	// Set up light buffer
	D3D11_BUFFER_DESC lightBufferDesc;
	ZeroMemory(&lightBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	lightBufferDesc.ByteWidth = sizeof(Light) * Constants::NUM_LIGHTS;
	lightBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	lightBufferDesc.StructureByteStride = sizeof(Light);

	// Randomly dist lights in scene
	RandomlyDistributeLights();

	D3D11_SUBRESOURCE_DATA lightInitData;
	lightInitData.pSysMem = &m_lights[0];
	lightInitData.SysMemPitch = 0;
	lightInitData.SysMemSlicePitch = 0;

	device->CreateBuffer(&lightBufferDesc, &lightInitData, &m_lightBuffer);
	
	// create srv to bind to pixel shader
	D3D11_SHADER_RESOURCE_VIEW_DESC lightSRVDesc;
	ZeroMemory(&lightSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	lightSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	lightSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	lightSRVDesc.BufferEx.FirstElement = 0;
	lightSRVDesc.BufferEx.NumElements = Constants::NUM_LIGHTS;

	device->CreateShaderResourceView(m_lightBuffer, &lightSRVDesc, &m_lightSRV);

	// Set up light index list
	D3D11_BUFFER_DESC lightIndexBufferDesc;
	ZeroMemory(&lightIndexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	lightIndexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightIndexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	lightIndexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	lightIndexBufferDesc.StructureByteStride = sizeof(int);
	lightIndexBufferDesc.ByteWidth = sizeof(int) * Constants::LIGHT_INDEX_COUNT;
	lightIndexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateBuffer(&lightIndexBufferDesc, nullptr, &m_lightIndexBuffer);

	D3D11_SHADER_RESOURCE_VIEW_DESC lightIndexSRVDesc;
	ZeroMemory(&lightIndexSRVDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	lightSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
	lightSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	lightSRVDesc.BufferEx.FirstElement = 0;
	lightSRVDesc.BufferEx.NumElements = Constants::LIGHT_INDEX_COUNT;

	device->CreateShaderResourceView(m_lightIndexBuffer, &lightIndexSRVDesc, &m_lightIndexBufferView);

	// Create 3D texture for cluster
	D3D11_TEXTURE3D_DESC clusterTexDesc;
	ZeroMemory(&clusterTexDesc, sizeof(D3D11_TEXTURE3D_DESC));
	clusterTexDesc.Width = Constants::NUM_X_CLUSTERS;
	clusterTexDesc.Height = Constants::NUM_Y_CLUSTERS;
	clusterTexDesc.Depth = Constants::NUM_Z_CLUSTERS;
	clusterTexDesc.MipLevels = 1;
	clusterTexDesc.Format = DXGI_FORMAT_R32G32_UINT;
	clusterTexDesc.Usage = D3D11_USAGE_DYNAMIC;
	clusterTexDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	clusterTexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	device->CreateTexture3D(&clusterTexDesc, nullptr, &m_clusterOffsetTex);

	D3D11_SHADER_RESOURCE_VIEW_DESC clusterViewDesc;
	ZeroMemory(&clusterViewDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	clusterViewDesc.Format = clusterTexDesc.Format;
	clusterViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
	clusterViewDesc.Texture3D.MipLevels = 1;

	device->CreateShaderResourceView(m_clusterOffsetTex, &clusterViewDesc, &m_clusterIndexOffsetView);
	
}

void LightManager::BindBuffer(ID3D11DeviceContext* context)
{
	D3D11_MAPPED_SUBRESOURCE lightResource;

	context->Map(m_lightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &lightResource);
	memcpy(lightResource.pData, &m_lights[0], Constants::NUM_LIGHTS * sizeof(Light));
	context->Unmap(m_lightBuffer, 0);

	/*D3D11_MAPPED_SUBRESOURCE clusterIndexOffsetSub;

	context->Map(m_clusterOffsetTex, 0, D3D11_MAP_WRITE_DISCARD, 0, &clusterIndexOffsetSub);
	memcpy(clusterIndexOffsetSub.pData, &m_clusterIndexOffset[0], (32) * (Constants::NUM_Y_PLANES - 1) * (Constants::NUM_Z_PLANES - 1) * sizeof(XMUINT2));
	context->Unmap(m_clusterOffsetTex, 0);

	D3D11_MAPPED_SUBRESOURCE lightBufferSubresource;
	context->Map(m_lightIndexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &lightBufferSubresource);
	memcpy(lightBufferSubresource.pData, &m_lightIndexList[0], Constants::LIGHT_INDEX_COUNT * sizeof(uint32_t));
	context->Unmap(m_lightIndexBuffer, 0);*/

	context->PSSetShaderResources(5, 1, &m_lightSRV);

	
}

void LightManager::Update(float dt)
{
	float rotation = dt * DirectX::XM_PIDIV2;

	XMMATRIX rot = XMMatrixRotationY(rotation);
	for (int i = 0; i < Constants::NUM_LIGHTS; i++)
	{
		XMStoreFloat3(&m_lights[i].m_positionWS, XMVector3Transform(XMLoadFloat3(&m_lights[i].m_positionWS), rot));
	}
}

void LightManager::RandomlyDistributeLights()
{
	XMFLOAT3 max = XMFLOAT3(17.9990796f, 14.2943311f, 11.8280701f);
	XMFLOAT3 min = XMFLOAT3(-19.2094580f, -12.6442505f, -11.0542590f);

	float range = 10.0f;

	for (int i = 0; i < Constants::NUM_LIGHTS; i++)
	{
		m_lights[i].m_positionWS = XMFLOAT3(GetRandomFloat(min.x, max.x), GetRandomFloat(min.y, max.z), GetRandomFloat(min.z, max.z));
		m_lights[i].m_directionWS = XMFLOAT4(0, -1, 0, 0);
		m_lights[i].m_color = GetRandomColor();
		m_lights[i].m_range = 10.0f;
		m_lights[i].m_spotAngle = 45.0f;
		m_lights[i].m_enabled = true;
		m_lights[i].m_type = LightType::Point;
	}

}

void LightManager::BuildClusters(XMFLOAT3 cameraPosition, XMFLOAT3 cameraLook, XMFLOAT3 cameraRight, XMFLOAT3 cameraUp)
{

	XMVECTOR camPos = XMLoadFloat3(&cameraPosition);
	XMVECTOR camLook = XMLoadFloat3(&cameraLook);
	XMVECTOR camRight = XMLoadFloat3(&cameraRight);
	XMVECTOR camUp= XMLoadFloat3(&cameraUp);

	m_frustumHeight = (float)std::tan(Constants::FOV * 0.5f);
	m_frustumWidth = m_frustumHeight * ((float)Constants::CLIENT_WIDTH / (float)Constants::CLIENT_HEIGHT);

	for (int x = 0; x < Constants::NUM_X_CLUSTERS; x++)
	{
		for (int y = 0; y < Constants::NUM_Y_CLUSTERS; y++)
		{
			for (int z = 0; z < Constants::NUM_Z_CLUSTERS; z++)
			{
				m_clusterLightList[0 + Constants::MAX_LIGHTS_PER_CLUSTER *
					x + Constants::MAX_LIGHTS_PER_CLUSTER * Constants::NUM_X_CLUSTERS *
					y + Constants::MAX_LIGHTS_PER_CLUSTER * Constants::NUM_X_CLUSTERS *
					Constants::NUM_Y_CLUSTERS * z] = 1;
			}
		}
	}

	XMVECTOR right = m_frustumWidth * Constants::FARZ * camRight ;
	XMVECTOR top = m_frustumHeight * Constants::FARZ * camUp;
	XMVECTOR farCentre = camPos + Constants::FARZ * camLook;
	XMVECTOR nearCentre = camPos + Constants::NEARZ * camLook;

	XMVECTOR topLeft = farCentre - right + top;
	XMVECTOR topRight = farCentre + right + top;
	XMVECTOR bottomLeft = farCentre - right - top;
	XMVECTOR bottomRight = farCentre + right - top;

	XMVECTOR leftToRight = (topRight - topLeft) / (float)(Constants::NUM_X_CLUSTERS);
	XMVECTOR bottomToTop = (topRight - bottomRight) / (float)(Constants::NUM_Y_CLUSTERS);
	XMVECTOR nearToFar = (farCentre - nearCentre) / (float)(Constants::NUM_Z_CLUSTERS);

	

	// March out x planes top and bottom points by uniform factor of leftToRight
	// And set normal and position
	XMVECTOR topPoint = topLeft;
	XMVECTOR bottomPoint = bottomLeft;
	for (unsigned i = 0; i < Constants::NUM_X_PLANES; i++)
	{
		
		XMVECTOR v1 = topPoint - camPos;
		XMVECTOR v2 = bottomPoint - camPos;

		  XMStoreFloat3(&m_xplanes[i].normal, XMVector3Normalize(XMVector3Cross(v1, v2)));
		  XMStoreFloat3(&m_xplanes[i].position, camPos);

		  topPoint += leftToRight;
		  bottomPoint += leftToRight;
	}

	// March out y planes left and right points up by uniform factor of bottomToTop 
	// And set normal and position 
	XMVECTOR leftPoint = bottomLeft;
	XMVECTOR rightPoint = bottomRight;
	for (unsigned i = 0; i < Constants::NUM_Y_PLANES; i++)
	{
		XMVECTOR v1 = leftPoint - camPos;
		XMVECTOR v2 = rightPoint - camPos;

		XMStoreFloat3(&m_yplanes[i].normal, XMVector3Normalize(XMVector3Cross(v1, v2)));
		XMStoreFloat3(&m_yplanes[i].position, camPos);

		leftPoint += bottomToTop;
		rightPoint += bottomToTop;
	}

	// March out z planes by uniform factor of nearToFar 
	// And set normal and position
	XMVECTOR nearPoint = nearCentre;
	for (unsigned i = 0; i < Constants::NUM_Z_PLANES; i++)
	{
		XMStoreFloat3(&m_zplanes[i].normal, camLook);
		XMStoreFloat3(&m_zplanes[i].position, nearPoint);

		nearPoint += nearToFar;
	}

}

void LightManager::CheckIntersection()
{
	// Loop through all lights in scene
	for (unsigned lightID = 0; lightID < Constants::NUM_LIGHTS; lightID++)
	{
		int leftXPlane = 0;
		int rightXPlane = 0;
		int bottomYPlane = 0;
		int topYPlane = 0;
		int nearZPlane = 0;
		int farZPlane = 0;

		float range = m_lights[lightID].m_range;
		XMVECTOR lightPosition = XMLoadFloat3(&m_lights[lightID].m_positionWS);

		// LH side plane
		XMVECTOR LHPlaneXNorm = XMLoadFloat3(&m_xplanes[0].normal);
		XMVECTOR LHPlaneXPos = XMLoadFloat3(&m_xplanes[0].position);
		// RH side plane
		XMVECTOR RHPlaneXNorm = XMLoadFloat3(&m_xplanes[Constants::NUM_X_PLANES - 1].normal);
		XMVECTOR RHPlaneXPos = XMLoadFloat3(&m_xplanes[Constants::NUM_X_PLANES - 1].position);
		// Bottom Plane
		XMVECTOR BPlaneYNorm = XMLoadFloat3(&m_yplanes[0].normal);
		XMVECTOR BPlaneYPos = XMLoadFloat3(&m_yplanes[0].position);
		// Top plane
		XMVECTOR TPlaneYNorm = XMLoadFloat3(&m_yplanes[Constants::NUM_Y_PLANES - 1].normal);
		XMVECTOR TPlaneYPos = XMLoadFloat3(&m_yplanes[Constants::NUM_Y_PLANES - 1].position);
		// Near z plane
		XMVECTOR nearPlaneNorm = XMLoadFloat3(&m_zplanes[0].normal);
		XMVECTOR nearPlanePos = XMLoadFloat3(&m_zplanes[0].position);

		// Far z plane
		XMVECTOR farPlaneNorm = XMLoadFloat3(&m_nearPlane.normal);
		XMVECTOR farPlanePos = XMLoadFloat3(&m_nearPlane.position);


		//Check if light sphere is outside outer frustum
		// If so skip back to top of loop
		if (XMVectorGetX(XMVector3Dot(-LHPlaneXNorm, (lightPosition - LHPlaneXNorm))) >= range ||
			XMVectorGetX(XMVector3Dot(-BPlaneYNorm, (lightPosition - BPlaneYPos))) >= range ||
			XMVectorGetX(XMVector3Dot(-nearPlaneNorm, (lightPosition - nearPlanePos))) >= range ||
			XMVectorGetX(XMVector3Dot(RHPlaneXNorm, (lightPosition - RHPlaneXPos))) >= range ||
			XMVectorGetX(XMVector3Dot(TPlaneYNorm, (lightPosition - TPlaneYPos))) >= range ||
			XMVectorGetX(XMVector3Dot(farPlaneNorm, (lightPosition - farPlanePos))) >= range)
			continue;


		// Get left and right x planes that the light range is within
		for (unsigned i = 1; i < Constants::NUM_X_PLANES; i++)
		{
			XMVECTOR LNorm = XMLoadFloat3(&m_xplanes[i].normal);
			XMVECTOR LPos = XMLoadFloat3(&m_xplanes[i].position);
			float d = XMVectorGetX(XMVector3Dot(LNorm, (lightPosition + (-LNorm * range) - LPos)));

			if (d < 0.0f)
			{
				leftXPlane = i - 1;
				for (unsigned j = i; j < Constants::NUM_X_PLANES; j++)
				{
					XMVECTOR RNorm = XMLoadFloat3(&m_xplanes[i].normal);
					XMVECTOR RPos = XMLoadFloat3(&m_xplanes[i].position);
					d = XMVectorGetX(XMVector3Dot(RNorm, (lightPosition + (RNorm * range) - RPos)));

					rightXPlane = j - 1;
					if (d < 0.0f)
						break;
				}
				break;
			}
		}

		// Get top and bottom y planes that the light range is within
		for (unsigned i = 1; i < Constants::NUM_Y_PLANES; i++)
		{
			XMVECTOR BNorm = XMLoadFloat3(&m_yplanes[i].normal);
			XMVECTOR BPos = XMLoadFloat3(&m_yplanes[i].position);
			float d = XMVectorGetX(XMVector3Dot(BNorm, (lightPosition + (-BNorm * range) - BPos)));

			if (d < 0.0f)
			{
				bottomYPlane = i - 1;
				for (unsigned j = i; j < Constants::NUM_X_PLANES; j++)
				{
					XMVECTOR TNorm = XMLoadFloat3(&m_yplanes[i].normal);
					XMVECTOR TPos = XMLoadFloat3(&m_yplanes[i].position);
					d = XMVectorGetX(XMVector3Dot(TNorm, (lightPosition + (TNorm * range) - TPos)));

					topYPlane = j - 1;
					if (d < 0.0f)
						break;
				}
				break;
			}
		}

		// Get near and far z planes that the light range is within
		for (unsigned i = 1; i < Constants::NUM_Z_PLANES; i++)
		{
			XMVECTOR NNorm = XMLoadFloat3(&m_zplanes[i].normal);
			XMVECTOR NPos = XMLoadFloat3(&m_zplanes[i].position);
			float d = XMVectorGetX(XMVector3Dot(NNorm, (lightPosition + (-NNorm * range) - NPos)));

			if (d < 0.0f)
			{
				nearZPlane = i - 1;
				for (unsigned j = i; j < Constants::NUM_X_PLANES; j++)
				{
					XMVECTOR FNorm = XMLoadFloat3(&m_zplanes[i].normal);
					XMVECTOR FPos = XMLoadFloat3(&m_zplanes[i].position);
					d = XMVectorGetX(XMVector3Dot(FNorm, (lightPosition + (FNorm * range) - FPos)));

					farZPlane = j - 1;
					if (d < 0.0f)
						break;
				}
				break;
			}
		}

		// Fill temp clusters with light
		for (int x = leftXPlane; x <= rightXPlane; x++)
		{
			for (int y = bottomYPlane; y <= topYPlane; y++)
			{
				for (int z = nearZPlane; z <= farZPlane; z++)
				{
					unsigned index = Constants::MAX_LIGHTS_PER_CLUSTER * x + Constants::MAX_LIGHTS_PER_CLUSTER * Constants::NUM_X_CLUSTERS * y + 
						Constants::MAX_LIGHTS_PER_CLUSTER *  Constants::NUM_X_CLUSTERS * Constants::NUM_Y_CLUSTERS * z;
					if (m_clusterLightList[index] < Constants::MAX_LIGHTS_PER_CLUSTER)
					{
						m_clusterLightList[index + m_clusterLightList[index]++] = lightID;
					}
				}
			}
		}
	}

	unsigned indexPos = 0;

	for (int x = 0; x < Constants::NUM_X_PLANES - 1; x++)
	{
		for (int y = 0; y < Constants::NUM_Y_PLANES - 1; y++)
		{
			for (int z = 0; z < Constants::NUM_Z_PLANES - 1; z++)
			{
				unsigned index = Constants::MAX_LIGHTS_PER_CLUSTER * x + Constants::MAX_LIGHTS_PER_CLUSTER * Constants::NUM_X_CLUSTERS * y +
					Constants::MAX_LIGHTS_PER_CLUSTER * Constants::NUM_X_CLUSTERS * Constants::NUM_Y_CLUSTERS * z;
				m_clusterIndexOffset[x + (32) * y + (32) * (Constants::NUM_Y_PLANES - 1) * z] = XMUINT2(indexPos, m_clusterLightList[index] - 1);

				for (int i = 1; i < m_clusterLightList[index]; i++)
				{
					m_lightIndexList[indexPos++] = m_clusterLightList[index + 1];
				}
			}
		}
	}
}
