#pragma once
#include "ConstantVars.h"




class LightManager
{
public:
	LightManager();
	~LightManager();

	void InitBuffers(ID3D11Device* device);
	void BindBuffer(ID3D11DeviceContext* context);
	void Update(float dt);

	void BuildClusters(XMFLOAT3 cameraPosition, XMFLOAT3 cameraLook, XMFLOAT3 cameraRight, XMFLOAT3 cameraUp );
	void CheckIntersection();


private:

	enum class LightType : uint32_t
	{
		Point = 0,
		Spot = 1,
		Directional = 2
	};

	struct Light
	{
		XMFLOAT3 m_positionWS;
		float m_pad;
		//(16b)
		XMFLOAT4 m_directionWS;
		//(16b)
		XMFLOAT4 m_color;
		//(16b)
		float m_range;
		float m_spotAngle;
		uint32_t m_enabled;
		LightType m_type;
		//(16b)
		//(16 * 5 = 80b)

		Light()
			: m_positionWS(0, 0, 0)
			, m_directionWS(0, 0, -1, 1)
			, m_color(1, 1, 1, 1)
			, m_range(100.0f)
			, m_spotAngle(45.0f)
			, m_enabled(true)
			, m_type(LightType::Point)
		{
			ZeroMemory(this, sizeof(this));
		}
	};


	void RandomlyDistributeLights();

	ID3D11Buffer* m_lightBuffer;

	ID3D11ShaderResourceView* m_lightSRV;

	Light m_lights[Constants::NUM_LIGHTS];

	ID3D11Buffer* m_lightIndexBuffer;
	ID3D11ShaderResourceView* m_lightIndexBufferView;
	ID3D11Texture3D* m_clusterOffsetTex;
	ID3D11ShaderResourceView* m_clusterIndexOffsetView;

	Plane m_xplanes[Constants::NUM_X_CLUSTERS + 1];
	Plane m_yplanes[Constants::NUM_Y_CLUSTERS + 1];
	Plane m_zplanes[Constants::NUM_Z_CLUSTERS + 1];
	Plane m_nearPlane;
	Plane m_farPlane;

	float m_frustumHeight;
	float m_frustumWidth;

	float m_frustumDepth;
	float m_clusterDepth;

	std::vector<short> m_clusterLightList;
	std::vector<int> m_lightIndexList;

	XMUINT2 m_clusterIndexOffset[(32) *  (Constants::NUM_Y_CLUSTERS + 1) * (Constants::NUM_Z_CLUSTERS + 1)];

	
};