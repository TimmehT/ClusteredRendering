#pragma once

#include "Direct3DManager.h"
#include "Camera.h"
#include "InputManager.h"
#include "Keycodes.h"

const bool g_enableVSync = false;
const bool g_enableFullScreen = false;
const float g_screenDepth = 1000.0f;
const float g_screenNear = 0.1f;

class GraphicsManager
{
public:
	GraphicsManager();
	GraphicsManager(const GraphicsManager&);
	~GraphicsManager();

	bool Initialize(unsigned __int16 clientWidth, unsigned __int16 clientHeight, HWND windowHandle);
	void Cleanup();
	bool Update(InputManager* input, float deltaTime);
	bool Render();
	void OnResize(unsigned __int16 clientWidth, unsigned __int16 clientHeight);

private:
	Direct3DManager* m_direct3D;
	Camera* m_camera;
};