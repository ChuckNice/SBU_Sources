#ifndef __BUTTON_TYPES__
#define __BUTTON_TYPES__

#include "button.h"
#include "cannon.h"

class CannonButton : public Button
{
public:
	CannonButton(Scene *scene, Vector3 size, float refreshTime, Cannon *cannon) : Button(scene, size, refreshTime), 
		m_cannon(cannon) { }

	void onPressed() {
		m_cannon->shootCannon();
	}

private:
	Cannon *m_cannon;
};

#endif