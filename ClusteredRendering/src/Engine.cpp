#include <DirectXPCH.h>
#include "Engine.h"

Engine* engine = 0;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return engine->WndProc(hwnd, message, wParam, lParam);
}

Engine::Engine()
{
}

Engine::Engine(const Engine& other)
{
}

Engine::~Engine()
{
}

int Engine::Initialize(HINSTANCE hinst, int cmdShow)
{
	m_windowWidth = 1280;
	m_windowHeight = 720;
	m_windowClassName = "DirectXWindowClass";
	m_windowName = "Clustered Rendering Engine";
	m_windowHandle = 0;

	if (InitializeApplication(hinst,cmdShow) != 0)
	{
		return -1;
	}
	return 0;
}

void Engine::Uninitialize()
{
	if (m_graphicsManager)
	{
		m_graphicsManager->Uninitialize();
		delete m_graphicsManager;
		m_graphicsManager = 0;
	}

	return;
}

int Engine::Run()
{
	MSG msg = { 0 };

	m_timer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			m_timer.Tick();
			ProcessFrame();

		}
	}

	return static_cast<int>(msg.wParam);
}

LRESULT Engine::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	PAINTSTRUCT paintStruct;
	HDC hDC;

	switch (message)
	{
	case WM_PAINT:
	{
		hDC = BeginPaint(hwnd, &paintStruct);
		EndPaint(hwnd, &paintStruct);
	}
	break;
	case WM_DESTROY:
	{
		PostQuitMessage(0);
	}
	break;
	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

HWND Engine::GetWindowHandle() const
{
	return HWND();
}

HINSTANCE Engine::GetAppInstance() const
{
	return HINSTANCE();
}

int Engine::InitializeApplication(HINSTANCE hInstance, int cmdShow)
{

	engine = this;

	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = MainWndProc;
	wndClass.hInstance = hInstance;
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = m_windowClassName;

	if (!RegisterClassEx(&wndClass))
	{
		return -1;
	}

	RECT windowRect = { 0, 0, m_windowWidth, m_windowHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	m_windowHandle = CreateWindowA(m_windowClassName, m_windowName,
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);

	if (!m_windowHandle)
	{
		return -1;
	}

	ShowWindow(m_windowHandle, cmdShow);
	UpdateWindow(m_windowHandle);

	return 0;
}

bool Engine::ProcessFrame()
{
	bool result;

	result = m_graphicsManager->ProcessFrame();
	if (!result)
	{
		return false;
	}
	return false;
}

void Engine::UnitizializeApplication()
{
	// Show mouse cursor
	ShowCursor(true);

	DestroyWindow(m_windowHandle);
	m_windowHandle = NULL;

	// Remove application instance
	UnregisterClass(m_windowClassName, m_appInstance);
}
