#ifndef __THE_MOUSE__
#define __THE_MOUSE__

/*
This class is a singleton that manages the mouse. It maintains state
for the two mouse buttons and can be queried for buttons being pressed
and held
*/
class Mouse
{
public:
	Mouse();

public:
	// most classes will use these methods to get mouse movement (relative to last frame) and state
	void getRelativeMousePos(int& x, int& y);
	bool isButtonPressed(int button);
	bool isButtonHeld(int button);

	// these methods get and set absolute mouse position on the screen
	void getMousePos(int& x, int& y);
	void setMousePos(int x, int y);
    void showCursor(bool val);
    void setMouseLocked(bool locked) { m_mouseLocked = locked; }

    void update(float dt);

private:
	bool m_buttonState[2];
    bool m_lastButtonState[2];
	float m_relativeMousePosX, m_relativeMousePosY;
    bool m_mouseLocked;
};

// external reference to singleton accessor
extern Mouse* s_Mouse();

#endif