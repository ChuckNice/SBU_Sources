#ifndef __NICK_SCENE__
#define __NICK_SCENE__

#include "global.h"
#include "button_types.h"
#include "OBJmodel.h"
#include "scene_graph.h"
#include "particle_types.h"
#include "player.h"

class NickButton;
class FractureObject;
class Cannon;
class ParticleSystem;

/**
 This is Nick's level for CSE381 Final Project
 */
class NickScene : public Scene
{
public:
	NickScene();
	bool resetPlayer();
    
protected:
	bool onInitialize();
	void onUpdate(float dt);
    void onRender();
	void onShutdown();
    bool shouldShowScore() { return false; }
    
private:
    FractureObject *m_globeFracture;
    vector<Cannon*> m_cannons;
    Cannon *m_globeCannon;
    ParticleSystem *m_playerParticleSystem;
    
public:
    void stopCannonsHit(NickButton *button);
    void shootGlobeHit(NickButton *button);
};



typedef void (NickScene::*NickButtonFunction)(NickButton * button);

/**
 This is a helper class that defines a button class that can call member functions
 of NickScene
 */
class NickButton : public Button
{
public:
	NickButton(NickScene *scene, const NickButtonFunction& function, Vector3 size, float refreshTime = INFINITE_REFRESH_TIME) :
    Button(scene, size, refreshTime), nick_scene(scene), m_function(function) { }
    
	void onPressed() {
		if (m_function) {
            (nick_scene->*m_function)(this);
        }
	}
    
private:
    NickScene *nick_scene;
	NickButtonFunction m_function;
};



// this is a subclass of OBJModel that uses hydralisk model to breath fire
class FireBreather : public OBJmodel
{
public:
    FireBreather(Scene *scene, float scale, float onTime, float offTime) :
    OBJmodel(scene, "Objects/hydralisk.obj", scale, false, false, true),
    m_onTime(onTime), m_offTime(offTime), m_timer(offTime), m_isOn(false), m_particleSystem(NULL) { }
    EntityType getType(void){ return ET_FIREBREATHER; }
    
    void attachParticleSystem(ParticleSystem *system) {
        m_particleSystem = system;
        m_particleSystem->emitterOrientation = Vector3(getYaw() + 90.0f, -40.0f, 0.0f);
        m_particleSystem->emitterOrientationVar = Vector3(10.0f, 20.0f, 0.0f);
        m_particleSystem->speed = 10.0f;
        m_origEmission = m_particleSystem->emissionRate;
        m_particleSystem->emissionRate = 0.0f;
        
        Vector3 dir = s_Cam()->calculateLookDir(getYaw() + 90.0f, 0.0f);
        dir.x *= (m_scale * 0.8f);
        dir.y += (m_scale * 0.1f);
        dir.z *= (m_scale * 0.8f);
        Vector3 pos = getPosition() + dir;
        m_particleSystem->moveToPoint(pos.x, pos.y, pos.z);
        m_particleSystem->startSystem();
        m_scene->getGraph()->attach(m_particleSystem);
    }
    
    void onPrepare( float dt )
    {
        m_timer -= dt;
        if (m_timer <= 0.0f) {
            m_isOn = !m_isOn;
            m_timer = m_isOn ? m_onTime : m_offTime;
        }
        
        if (m_isOn)
            m_particleSystem->emissionRate = m_origEmission;
        else
            m_particleSystem->emissionRate = 0.0f;
    }
    
private:
    float m_onTime;
    float m_offTime;
    float m_timer;
    bool m_isOn;
    float m_origEmission;
    ParticleSystem *m_particleSystem;
};

#endif