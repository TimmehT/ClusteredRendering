#pragma once

class InputManager
{
public:

	InputManager();
	~InputManager();

	// Keyboard Windows Messaging
	bool KeyPressed(const int key);
	bool KeyDown(const int key)const;


	void SetKeyState(const int key, const bool state);

	// Mouse RAW
	int GetMouseDiffX()const;
	int GetMouseDiffY()const;
	int GetMouseScrollPos()const;

	bool MouseMoved()const;
	bool MouseButtonPressed(const int button);
	bool MouseButtonDown(const int button)const;
	bool MouseScrolled()const;

	bool InitializeRawMouse(const HWND hwnd);
	void UpdateMouse();

	BYTE* GetMouseBuffer();

	void Reset();

	bool Initialized()const;

	void Release();

private:

	// Keyboard
	bool m_keyDown[256];
	bool m_keyPressed[256];

	// Mouse
	RAWINPUT* m_rawInput;
	RAWINPUTDEVICE m_rawID[1];

	BYTE* m_mouseBuffer;
	long  m_mouseDiffX;
	long  m_mouseDiffY;
	bool m_mouseMoved;
	bool m_mouseButtonDown[3];
	bool m_mouseButtonPressed[3];
	bool m_mouseScrolled;
	int m_mouseScrollPos;
	int m_lastMouseScrollPos;
	bool m_initialized;

	long m_mouseMovedX;
	long m_mouseMovedY;

	

};