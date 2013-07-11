#ifndef __SCENE_H__
#define __SCENE_H__

#include "global.h"
#include <algorithm>

class SceneGraph;
class Player;
class btDynamicsWorld;
class btDefaultCollisionConfiguration;
class btCollisionDispatcher;
class btBroadphaseInterface;
class btConstraintSolver;
class FractureObject;
class Waypoint;
class Start;

/**
This class is the base class for scene. A scene can be thought of as a world
or level. Each scene contains a scene graph that is comprised of the scene's
entities. Scenes are parsed from files.
*/
class Scene
{
public:
	Scene();
	virtual ~Scene();

	bool initialize();
	virtual void update(float dt);
	virtual void render();
    void stepPhysics(float dt);
    void shutdown();
    
	virtual bool resetPlayer() = 0;
	void onSimulationTick(float dt);
	
	// pass an entity in this method to have it's onSimulationTick method called on each physics substep
	void registerForSimulationTicks(Entity *entity) { m_simulationTickEntities.push_back(entity); }
	void unregisterForSimulationTicks(Entity *entity) {
        m_simulationTickEntities.erase(find(m_simulationTickEntities.begin(), m_simulationTickEntities.end(), entity)); }
	void registerForSimulationTicks(FractureObject *fractureObj) { m_simulationFractureObjects.push_back(fractureObj); }

	// getters
	SceneGraph* getGraph() { return m_graph; }
	Player* getPlayer() { return m_player; }
	btDynamicsWorld* getDynamicsWorld() { return m_dynamicsWorld; }
    
    void finishedLevel() { m_congratz = true; }
	void setClearColor(float red, float green, float blue) { m_clearColor_r = red; m_clearColor_g = green; m_clearColor_b = blue;}

protected:
	// required virtual methods
	virtual bool onInitialize() = 0;
	virtual void onShutdown() = 0;

	// optional virtual methods
	virtual void onUpdate(float dt) { }
    virtual void onRender() { }
    
    virtual bool shouldShowScore() { return true; }

	SceneGraph *m_graph;
	FreeTypeFont *m_font;
	FreeTypeFont *m_congratFont;
	Player *m_player;

	std::vector<Entity *> m_simulationTickEntities;
	typedef vector<Entity*>::iterator EntityIterator;
    std::vector<FractureObject *> m_simulationFractureObjects;
	typedef vector<FractureObject*>::iterator FractureIterator;

	float m_FPS;
	void updateFPS(float dt);

	// bullet stuff
	btDynamicsWorld* m_dynamicsWorld;
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_broadphase;
	btConstraintSolver*	m_solver;

	bool m_wireframe;
	bool m_displayStats;
    bool m_congratz;

	vector<Waypoint*> m_allCheckpoints;

	Start *m_WPstart;
	Waypoint *m_WPgoal;
	float m_clearColor_r;
	float m_clearColor_g;
	float m_clearColor_b;
};


#endif