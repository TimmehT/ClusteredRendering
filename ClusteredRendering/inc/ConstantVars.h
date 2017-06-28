#pragma once

#include <DirectXMath.h>

namespace Constants
{
	const uint16_t CLIENT_WIDTH = 1280;
	const uint16_t CLIENT_HEIGHT = 720;
	const float FOV = XM_PIDIV4;
	const float FARZ = 3000.0f;
	const float NEARZ = 0.1f;

	const uint16_t NUM_X_CLUSTERS = 16;
	const uint16_t NUM_Y_CLUSTERS = 8;
	const uint16_t NUM_Z_CLUSTERS = 24;

	const uint16_t NUM_X_PLANES = NUM_X_PLANES + 1;
	const uint16_t NUM_Y_PLANES = NUM_Y_PLANES + 1;
	const uint16_t NUM_Z_PLANES = NUM_Z_PLANES + 1;

	const uint16_t NUM_CLUSTERS = NUM_X_CLUSTERS * NUM_Y_CLUSTERS * NUM_Z_CLUSTERS;

	const uint16_t NUM_LIGHTS = 50;
	const uint16_t MAX_LIGHTS_PER_CLUSTER = 30;
	const uint32_t LIGHT_INDEX_COUNT = (NUM_CLUSTERS * MAX_LIGHTS_PER_CLUSTER);
}

struct Plane
{
	Plane() {};
	Plane(XMFLOAT3 pos, XMFLOAT3 norm)
	{
		position = pos;
		normal = norm;
	}

	XMFLOAT3 position;
	XMFLOAT3 normal;
};
