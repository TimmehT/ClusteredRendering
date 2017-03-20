#include <DirectXPCH.h>
#include "GraphicsManager.h"

GraphicsManager::GraphicsManager()
{
}

GraphicsManager::GraphicsManager(const GraphicsManager& other)
{
}

GraphicsManager::~GraphicsManager()
{
}

bool GraphicsManager::Initialize(LONG windowWidth, LONG windowHeight, HWND windowHandle)
{
	bool result;
	// Create D3D object
	m_direct3D = new Direct3DManager;
	if (!m_direct3D)
	{
		return false;
	}

	if (!m_direct3D->Initialize())
	{
		MessageBox(windowHandle, TEXT("Could not initialize Direct3D"), TEXT("Error"), MB_OK);
		return false;
	}
	return false;
}

void GraphicsManager::Uninitialize()
{
}

bool GraphicsManager::ProcessFrame()
{
	if (!Render())
	{
		return false;
	}

	return true;
}

bool GraphicsManager::Render()
{
	return false;
}
