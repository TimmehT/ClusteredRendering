#include <DirectXPCH.h>
#include "InputManager.h"

InputManager::InputManager()
{
	m_rawInput = nullptr;
	m_mouseBuffer = nullptr;

	m_mouseMoved = false;
	m_mouseScrolled = false;

	m_mouseDiffX = 0;
	m_mouseDiffY = 0;

	m_mouseScrollPos = 0;
	m_lastMouseScrollPos = 0;

	m_initialized = false;
}

InputManager::~InputManager()
{
}

bool InputManager::KeyPressed(const int key) const
{
	return (!m_lastKeys[key] && m_keys[key]);
}

bool InputManager::KeyDown(const int key) const
{
	return m_keys[key];
}

void InputManager::SetKeyState(const int key, const bool state)
{
	m_lastKeys[key] = m_keys[key];
	m_keys[key] = state;
}

void InputManager::UpdateKeys()
{
	memcpy(m_lastKeys, m_keys, sizeof(m_keys));
}

int InputManager::GetMouseDiffX() const
{
	return m_mouseDiffX;
}

int InputManager::GetMouseDiffY() const
{
	return m_mouseDiffY;
}

int InputManager::GetMouseScrollPos() const
{
	return m_mouseScrollPos;
}

bool InputManager::MouseMoved() const
{
	return m_mouseMoved;
}

bool InputManager::MouseButtonPressed(const int button) const
{
	if (!m_lastMouseButtonDown[button] && m_mouseButtonDown[button])
		return true;
	else
		return false;
}

bool InputManager::MouseButtonDown(const int button) const
{
	return m_mouseButtonDown[button];
}

bool InputManager::MouseScrolled() const
{
	return m_mouseScrolled;
}

bool InputManager::InitializeRawMouse(const HWND hwnd)
{
	m_rawID[0].usUsagePage = 0x01;
	m_rawID[0].usUsage = 0x02;
	m_rawID[0].dwFlags = RIDEV_CAPTUREMOUSE | RIDEV_INPUTSINK | RIDEV_NOLEGACY;
	m_rawID[0].hwndTarget = hwnd;

	if (!RegisterRawInputDevices(m_rawID, 1, sizeof(RAWINPUTDEVICE)))
		return false;

	m_mouseBuffer = new BYTE[40];

	m_initialized = true;
	return m_initialized;
}

void InputManager::UpdateMouse()
{
	m_rawInput = nullptr;
	m_rawInput = (RAWINPUT*)m_mouseBuffer;

	if (m_rawInput != nullptr)
	{
		if (m_rawInput->header.dwType == RIM_TYPEMOUSE)
		{
			m_mouseScrolled = false;


			// Calc position
			m_mouseDiffX += m_rawInput->data.mouse.lLastX;
			m_mouseDiffY += m_rawInput->data.mouse.lLastY;
			m_mouseMoved = true;

			if (m_rawInput->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN)
			{
				m_lastMouseButtonDown[0] = m_mouseButtonDown[0];
				m_mouseButtonDown[0] = true;
			}

			if (m_rawInput->data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_UP)
			{
				m_lastMouseButtonDown[0] = m_mouseButtonDown[0];
				m_mouseButtonDown[0] = false;
			}

			if (m_rawInput->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN)
			{
				m_lastMouseButtonDown[1] = m_mouseButtonDown[1];
				m_mouseButtonDown[1] = true;
			}

			if (m_rawInput->data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_UP)
			{
				m_lastMouseButtonDown[1] = m_mouseButtonDown[1];
				m_mouseButtonDown[1] = false;
			}

			if (m_rawInput->data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN)
			{
				m_lastMouseButtonDown[2] = m_mouseButtonDown[2];
				m_mouseButtonDown[2] = true;
			}

			if (m_rawInput->data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_UP)
			{
				m_lastMouseButtonDown[2] = m_mouseButtonDown[2];
				m_mouseButtonDown[2] = false;
			}

			if (m_rawInput->data.mouse.ulButtons & RI_MOUSE_WHEEL)
			{
				m_mouseScrolled = true;
				m_lastMouseScrollPos = m_mouseScrollPos;

				if (m_rawInput->data.mouse.usButtonData > 0)
					m_mouseScrollPos = 1;
				else
					m_mouseScrollPos = -1;
			}

		}
	}
}

BYTE * InputManager::GetMouseBuffer()
{
	return m_mouseBuffer;
}

void InputManager::Reset()
{
	m_mouseMoved = false;
	m_mouseScrolled = false;

	m_mouseButtonDown[0] = false;
	m_mouseButtonDown[1] = false;
	m_mouseButtonDown[2] = false;

	m_mouseDiffX = 0;
	m_mouseDiffY = 0;
}

bool InputManager::Initialized() const
{
	return m_initialized;
}

void InputManager::Release()
{
	SafeDelete(m_rawInput);

	SafeDeleteArray(m_mouseBuffer);
}
