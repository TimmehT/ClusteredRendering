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

bool GraphicsManager::Initialize(unsigned __int16 clientWidth, unsigned __int16 clientHeight, HWND windowHandle)
{

	// Create D3D object
	m_direct3D = new Direct3DManager;
	if (!m_direct3D)
	{
		return false;
	}

	if (!m_direct3D->Initialize(windowHandle, g_enableVSync))
	{
		MessageBox(windowHandle, TEXT("Could not initialize Direct3D"), TEXT("Error"), MB_OK);
		return false;
	}

	m_camera = new Camera;
	if (!m_camera)
	{
		return false;
	}


	return true;
}

void GraphicsManager::Cleanup()
{
	if (m_direct3D)
	{
		m_direct3D->Cleanup();
		SafeDelete(m_direct3D);
	}

	if (m_camera)
	{
		SafeDelete(m_camera);
	}
}

bool GraphicsManager::Update(InputManager* input,float deltaTime)
{
	if (input->Initialized())
	{
		{
			if (input->KeyDown(VK_W))
			{
				m_camera->MoveForward(deltaTime);
			}

			if (input->KeyDown(VK_S))
			{
				m_camera->MoveBackward(deltaTime);
			}

			if (input->KeyDown(VK_A))
			{
				m_camera->MoveLeft(deltaTime);
			}

			if (input->KeyDown(VK_D))
			{
				m_camera->MoveRight(deltaTime);
			}

			if (input->MouseScrolled())
			{
				if (input->GetMouseScrollPos() < 0)
				{
					m_camera->MoveBackward(deltaTime * 100);
				}
				if (input->GetMouseScrollPos() > 0)
				{
					m_camera->MoveForward(deltaTime * 100);
				}
			}

			if (input->MouseButtonDown(LMB))
			{
				m_camera->MoveUp(deltaTime);
			}

			if (input->MouseButtonDown(RMB))
			{
				m_camera->MoveDown(deltaTime);
			}

			if (input->MouseMoved())
			{
				m_camera->Pitch((input->GetMouseDiffY() * deltaTime));
				m_camera->Yaw((input->GetMouseDiffX() * deltaTime));

			}
		}
		input->Reset();
	}

	
	return true;
}

bool GraphicsManager::Render()
{
	m_direct3D->Clear(Colors::CornflowerBlue, 1.0f, 0);
	//m_direct3D->SetRasterizerStage();
	//m_direct3D->SetOutputMergerStage();
	m_direct3D->Present(g_enableVSync);

	return true;
}

void GraphicsManager::OnResize(unsigned __int16 clientWidth, unsigned __int16 clientHeight)
{
	m_camera->SetLens(XMConvertToRadians(68.0f), 0.1f, 100.0f, clientWidth, clientHeight);
}
