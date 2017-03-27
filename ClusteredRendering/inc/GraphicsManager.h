#pragma once

#include "Direct3DManager.h"
#include "Camera.h"

const BOOL g_enableVSync = FALSE;
const BOOL g_enableFullScreen = FALSE;
const float g_screenDepth = 1000.0f;
const float g_screenNear = 0.1f;

class GraphicsManager
{
public:
	GraphicsManager();
	GraphicsManager(const GraphicsManager&);
	~GraphicsManager();

	bool Initialize(LONG windowWidth, LONG windowHeight, HWND windowHandle);
	void Uninitialize();
	bool ProcessFrame(float);

private:
	bool Update(float);
	bool Render();

private:
	Direct3DManager* m_direct3D;
	Camera* m_camera;
};