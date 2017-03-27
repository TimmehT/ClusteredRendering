#include <DirectXPCH.h>
#include "GraphicsManager.h"

GraphicsManager::GraphicsManager()
{
	m_direct3D = nullptr;
	m_camera = nullptr;
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

	if (!m_direct3D->Initialize(g_enableVSync, windowHandle, g_enableFullScreen, g_screenDepth, g_screenNear))
	{
		MessageBox(windowHandle, TEXT("Could not initialize Direct3D"), TEXT("Error"), MB_OK);
		return false;
	}
	return false;
}

void GraphicsManager::Uninitialize()
{
	if (m_direct3D)
	{
		m_direct3D->Unitialize();
		delete m_direct3D;
		m_direct3D = nullptr;
	}
}

bool GraphicsManager::ProcessFrame(float deltaTime)
{
	if (!Update(deltaTime))
	{
		return false;
	}

	if (!Render())
	{
		return false;
	}

	return true;
}

bool GraphicsManager::Update(float deltaTime)
{
	return true;
}

bool GraphicsManager::Render()
{
	m_direct3D->Clear(Colors::CornflowerBlue, 1.0f, 0);
	m_direct3D->SetRasterizerStage();
	m_direct3D->SetOutputMergerStage();
	m_direct3D->Present(g_enableVSync);

	return true;
}
