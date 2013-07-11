
#ifndef __SCENE_MANAGER__
#define __SCENE_MANAGER__

#include "scene.h"

class SceneManager
{
public:
    SceneManager();
    
    bool loadScene(Scene *newScene);
    
    void update(float dt);
    void stepPhysics(float dt);
    void render();
    
    void shutdown();

	Scene* getCurrentScene() { return m_currentScene; }
    
private:
    Scene *m_currentScene;
};

// external reference to singleton accessor
extern SceneManager* s_SceneManager();

#endif
