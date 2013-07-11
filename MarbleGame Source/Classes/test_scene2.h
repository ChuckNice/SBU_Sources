#ifndef __TEST_SCENE2__
#define __TEST_SCENE2__

#include "global.h"

/**
This class is a hardcoded test scene (not loaded from file)
*/
class TestScene2 : public Scene
{
public:
	TestScene2();
	bool resetPlayer();

protected:
	bool onInitialize();
	void onUpdate(float dt);
	void onShutdown();

};

#endif