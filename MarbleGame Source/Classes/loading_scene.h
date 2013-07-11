#ifndef __LOADING_SCENE__
#define __LOADING_SCENE__

#include "global.h"

/**
This scene is displayed while another scene is loading
*/
class LoadingScene : public Scene
{
public:
	LoadingScene();
	void render();
	bool resetPlayer() { return true; }

protected:
	bool onInitialize();
	void onShutdown();

	FreeTypeFont *m_loadingFont;
};

#endif