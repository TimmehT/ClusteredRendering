#pragma once
#include "GameTimer.h"
#include "GraphicsManager.h"

class Engine
{
public:
	Engine();
	Engine(const Engine&);
	~Engine();

	int Initialize(HINSTANCE hinst, int cmdShow);
	void Uninitialize();
	int Run();

	LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

	HWND GetWindowHandle()const;
	HINSTANCE GetAppInstance()const;

private:
	int InitializeApplication(HINSTANCE hInstance, int cmdShow);
	bool ProcessFrame(float);
	void OnResize();
	void UnitizializeApplication();

private:
	LONG m_windowWidth;
	LONG m_windowHeight;
	LPCSTR m_windowClassName;
	LPCSTR m_windowName;
	HWND m_windowHandle;
	HINSTANCE m_appInstance;
	bool m_appPaused;
	bool m_minimized;
	bool m_maximized;
	bool m_resizing;

	GameTimer m_timer;
	GraphicsManager* m_graphicsManager;

};

