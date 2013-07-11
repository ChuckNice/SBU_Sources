#ifndef __TEST_SCENE__
#define __TEST_SCENE__

#include "global.h"

/**
This class is a hardcoded test scene (not loaded from file)
*/
class TestScene : public Scene
{
public:
	TestScene();
	bool resetPlayer();

protected:
	bool onInitialize();
	void onUpdate(float dt);
    void onRender();
	void onShutdown();
};

#endif