
#ifndef __GUI_SCENE__
#define __GUI_SCENE__

#include "scene.h"

/**
 This subclass of scene defines a 2D GUI. When a 2D GUI scene is being
 rendered, the mouse is not constrained to the center of the screen or hidden
 so the user can interact with the screen.
 */
class GUIScene : public Scene
{
public:
    GUIScene();
    
    //void update(float dt);
    void render();
	virtual bool resetPlayer() { return true; }
    
    virtual bool onInitialize() = 0;
	virtual void onShutdown() = 0;
    
	virtual void onUpdate(float dt) = 0;
    virtual void onRender() = 0;
};


#endif
