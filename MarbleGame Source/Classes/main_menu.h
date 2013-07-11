
#ifndef __MAIN_MENU__
#define __MAIN_MENU__

#include "gui_scene.h"

class GUIImage;

class MainMenu : public GUIScene
{
public:
    MainMenu();
    
    bool onInitialize();
	void onShutdown();
    bool resetPlayer();
    
	void onUpdate(float dt);
    void onRender();
    
private:
    bool m_showHowToPlay;
    GUIImage *nicks_level;
    GUIImage *charlies_level;
	GUIImage *create_new_level;
    GUIImage *how_to_play_button;
    GUIImage *how_to_play;
};

#endif 
