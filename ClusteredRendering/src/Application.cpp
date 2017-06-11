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
	//InitializeWindows();

	m_input = new InputManager;
	if (!m_input)
		return false;

	m_graphics = new GraphicsManager;
	if (!m_graphics)
		return false;

	if (!m_graphics->Initialize(2, 2, m_windowHandle))
	{
		return false;
	}

	return true;
}

void Application::Cleanup()
{
	if (m_graphics)
	{
		m_graphics->Cleanup();
		SafeDelete(m_graphics);
	}

	if (m_input)
	{
		m_input->Release();
		SafeDelete(m_input);
	}

	ShutdownWindows();

	return;

}

int Application::Run()
{
	return 0;
}

LRESULT CALLBACK Application::MessageHandler(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDC;

	switch (message)
	{
	 case WM_CREATE:
	 {
		 m_input->InitializeRawMouse(hwnd);
	 }
	 break;
	 case WM_KEYDOWN:
	 {
		 m_input->SetKeyState(wParam, true);

		 if (m_input->KeyPressed(VK_ESCAPE))
		 {
			 PostMessage(hwnd, WM_QUIT, 0, 0);
		 }
	 }
	 break;
	 case WM_KEYUP:
	 {
		 m_input->SetKeyState(wParam, false);
	 }
	 break;
	 case WM_INPUT:
	 {

		 UINT bufferSize;
		 GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &bufferSize, sizeof(RAWINPUTHEADER));
		 GetRawInputData((HRAWINPUT)lParam, RID_INPUT, (LPVOID)m_input->GetMouseBuffer(), &bufferSize, sizeof(RAWINPUTHEADER));

		 m_input->UpdateMouse();

	 }
	 break;
	 case WM_PAINT:
	 {
		 hDC = BeginPaint(hwnd, &paintStruct);
		 EndPaint(hwnd, &paintStruct);
	 }
	 break;
	 default:
	 {
		 return DefWindowProc(hwnd, message, wParam, lParam);
	 }
	}
	return 0;
}

bool Application::InitializeWindows(HINSTANCE hInstance, int cmdShow)
{
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = &WndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = g_windowClassName;

	ShowCursor(false);

	if (!RegisterClassEx(&wndClass))
	{
		return false;
	}

	RECT windowRect = { 0, 0, m_windowWidth, m_windowHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	m_windowHandle = CreateWindowA(g_windowClassName, g_windowName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);

	if (!m_windowHandle)
	{
		return false;
	}

	ShowWindow(m_windowHandle, cmdShow);
	UpdateWindow(m_windowHandle);

	return true;
}

void Application::ShutdownWindows()
{
	ShowCursor(true);

	DestroyWindow(m_windowHandle);
	m_windowHandle = NULL;

	//UnregisterClass(m_win)
	m_hinstance = NULL;

	SafeDelete(ApplicationHandle);

	return;
}

void Application::OnResize()
{
}


LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}

		case WM_CLOSE:
		{
			PostQuitMessage(0);
			return 0;
		}

		default:
		{
			return ApplicationHandle->MessageHandler(hwnd, message, wParam, lParam);
		}
	}
}