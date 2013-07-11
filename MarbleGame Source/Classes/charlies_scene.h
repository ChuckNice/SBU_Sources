#ifndef __CHARLIE_SCENE__
#define __CHARLIE_SCENE__

#include "global.h"
#include "button_types.h"

class CharlieButton;
class FractureObject;
class Cannon;
class ParticleSystem;

/**
 This is Nick's level for CSE381 Final Project
 */
class CharlieScene : public Scene
{
public:
	CharlieScene();
	bool resetPlayer();
    
protected:
	bool onInitialize();
	void onUpdate(float dt);
    void onRender();
	void onShutdown();
    
private:
    //FractureObject *m_globeFracture;
    vector<Cannon*> m_cannons;
    //ParticleSystem *playerParticleSystem;
    
public:
    void globeButtonHit(CharlieButton *button);
    void stopCannonsHit(CharlieButton *button);
};



typedef void (CharlieScene::*CharlieButtonFunction)(CharlieButton * button);

/**
 This is a helper class that defines a button class that can call member functions
 of NickScene
 */
class CharlieButton : public Button
{
public:
	CharlieButton(CharlieScene *scene, const CharlieButtonFunction& function, Vector3 size, float refreshTime = INFINITE_REFRESH_TIME) :
    Button(scene, size, refreshTime), charlie_scene(scene), m_function(function) { }
    
	void onPressed() {
		if (m_function) {
            (charlie_scene->*m_function)(this);
        }
	}
    
private:
    CharlieScene *charlie_scene;
	CharlieButtonFunction m_function;
};

#endif