#pragma once

enum class LightType : uint32_t
{
	Point = 0,
	Spot = 1,
	Directional = 2
};

struct Light
{
	XMFLOAT4 m_positionWS;
	//(16b)
	XMFLOAT4 m_directionWS;
	//(16b)
	XMFLOAT4 m_color;
	//(16b)
	XMFLOAT3 m_attenuation;
	float m_specIntensity;
	//(16b)
	float m_range;
	float m_spotAngle;
	uint32_t m_enabled;
	LightType m_type;
	//(16b)
	//(16 * 5 = 80b)

	Light()
		: m_positionWS(0, 0, 0, 1)
		, m_directionWS(0, 0, -1, 0)
		, m_color(1, 1, 1, 1)
		, m_attenuation(1, 0, 0)
		, m_specIntensity(1.0f)
		, m_range(100.0f)
		, m_spotAngle(45.0f)
		, m_enabled(true)
		, m_type(LightType::Point)
	{
		ZeroMemory(this, sizeof(this));
	}
};

class LightManager
{
public:
	LightManager();
	~LightManager();


private:
};