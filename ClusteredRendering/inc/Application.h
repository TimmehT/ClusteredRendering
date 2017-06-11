#pragma once

#include "GameTimer.h"
#include "InputManager.h"
#include "GraphicsManager.h"

class Application
{
public:
	Application();
	Application(const Application&);
	~Application();

	bool Initialize();
	void Cleanup();
	int Run();

	LRESULT CALLBACK MessageHandler(HWND, UINT, WPARAM, LPARAM);


private:
	bool InitializeWindows(HINSTANCE hInstance, int cmdShow);
	void ShutdownWindows();
	void OnResize();

	HINSTANCE m_hinstance;
	HWND m_windowHandle;
	unsigned __int16 m_windowWidth;
	unsigned __int16 m_windowHeight;
	LPCSTR g_windowClassName;
	LPCSTR g_windowName;

	InputManager* m_input;
	GraphicsManager* m_graphics;
};


static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static Application* ApplicationHandle = nullptr;