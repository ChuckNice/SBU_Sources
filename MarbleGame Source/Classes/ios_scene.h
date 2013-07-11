#ifndef __IOS_SCENE__
#define __IOS_SCENE__

#include "global.h"

/**
This class is a hardcoded test scene (not loaded from file)
*/
class iOSScene : public Scene
{
public:
	iOSScene();
	bool resetPlayer();

protected:
	bool onInitialize();
	void onUpdate(float dt);
    void onRender();
	void onShutdown();
};

#endif