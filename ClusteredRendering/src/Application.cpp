#include <DirectXPCH.h>
#include "Application.h"

Application::Application()
{
	m_input = nullptr;
	m_graphics = nullptr;
}

Application::Application(const Application &)
{
}

Application::~Application()
{
}

bool Application::Initialize()
{
	return false;
}

void Application::Cleanup()
{
}

int Application::Run()
{
	return 0;
}

LRESULT CALLBACK Application::MessageHandler(HWND, UINT, WPARAM, LPARAM)
{
	return LRESULT();
}

bool Application::InitializeWindows()
{
	return false;
}

void Application::ShutdownWindows()
{
}

void Application::OnResize()
{
}
