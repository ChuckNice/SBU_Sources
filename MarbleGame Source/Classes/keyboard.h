#ifndef __KEY_BOARD__
#define __KEY_BOARD__

/*
This is a singleton class that manages the keyboard. It maintains
state for every key and can be queried for keys being
pressed and being held down
*/
class Keyboard
{
public:
	Keyboard();

public:
	// most classes will only use these methods
	bool isKeyPressed(const char key);
    bool isKeyHeldDown(const char key);

	// these methods are used by the glwindow to process keyboard events
    void handleKeyDown(unsigned int code);
    void handleKeyUp(unsigned int code);
	void update();

	// special key codes
	static const char KC_SPACE = (const char)32; // SPACE NOT WORKING
	static const char KC_UP = (const char)252;
	static const char KC_DOWN = (const char)253;
	static const char KC_RIGHT = (const char)254;
	static const char KC_LEFT = (const char)255;

private:
	//Translate a OS key to a regular char
    const char translateKey(unsigned int code);

	bool m_keyState[256];
    bool m_lastKeyState[256];
};

// external reference to singleton accessor
extern Keyboard* s_Keyboard();

#endif