#ifdef _WIN32
    #include <windows.h>
#endif

#include "keyboard.h"
#include <stdlib.h>
#include <memory.h>


// singleton getter for keyboard
Keyboard* s_Keyboard()
{
	static Keyboard s_keyboard;
	return &s_keyboard;
}


Keyboard::Keyboard()
{
	// clear keys
	for (int i = 0; i < 256; ++i) {
		m_keyState[i] = false;
		m_lastKeyState[i] = false;
	}
}


bool Keyboard::isKeyPressed(const char key)
{
	return (!m_lastKeyState[key] && m_keyState[key]);
}

bool Keyboard::isKeyHeldDown(const char key)
{
	return (m_lastKeyState[key] && m_keyState[key]);
}


void Keyboard::handleKeyDown(unsigned int code)
{
	const char key = translateKey(code);
	m_keyState[key] = true;
}

void Keyboard::handleKeyUp(unsigned int code)
{
	const char key = translateKey(code);
	m_keyState[key] = false;
}

void Keyboard::update()
{
	memcpy(m_lastKeyState, m_keyState, sizeof(bool) * 256);
}

const char Keyboard::translateKey(unsigned int code) //Translate a OS key to a KeyCode
{
	switch(code)
	{
#ifdef _WIN32
	case VK_UP:
		return KC_UP;
		break;
	case VK_DOWN:
		return KC_DOWN;
		break;
	case VK_LEFT:
		return KC_LEFT;
		break;
	case VK_RIGHT:
		return KC_RIGHT;
		break;
#endif

	default:
		return (const char)code;
	}
}