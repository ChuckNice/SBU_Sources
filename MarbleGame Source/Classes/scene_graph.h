#ifndef __SCENE_GRAPH__
#define __SCENE_GRAPH__

#include "global.h"
#include <list>

#define ROOT_NODE_BOUNDS 1000.0f
#define DEFAULT_WOLRD_LOWER_BOUND -100.0f

class ParticleSystem;

/**
This class is a scene graph for a scene. Scene attach all their entities to
the graph with the attach method. In each frame, the attached entities are
scene graph culled, then frustum culled, and finally rendered.
*/
class SceneGraph
{
public:
	SceneGraph(Scene *scene);
	~SceneGraph();

	// attach and detach entities. Note, entities might not detach until beginning of next frame.
	void attach(Entity *entity);
	void detach(Entity *entity);
    
    // attach and detach particle systems.
	void attach(ParticleSystem *system);

	// use these methods to push and pop nodes into the graph for scene graph culling
	void pushNode();
	void popNode();

	// this method will initiate scene graph culling, then frustum culling, and finally rendering
	void render();
	void update(float dt);

	// setters and getters
	void setWorldLowerBound(float lowerBound) { m_worldLowerBound = lowerBound; }
	int getNumEntitiesInScene() { return (int)m_entities.size(); }
	int getNumSceneGraphCulledEntities() { return m_numSceneGraphCulledEntities; }
	int getNumFrustumCulledEntities() { return m_numFrustumCulledEntities; }
	int getNumEntitiesRendered() { return m_numEntitiesRendered; }

private:
	Scene *m_scene;
	GraphNode *m_rootNode;
	GraphNode *m_currentNode;

	list<Entity*> m_entities;
	typedef list<Entity*>::iterator EntityIterator;
    
    vector<ParticleSystem*> m_particleSystems;
	typedef vector<ParticleSystem*>::iterator ParticleSystemIterator;

	void sceneGraphCull(list<Entity*> *inEntities, list<Entity*> *outEntities);
	void frustumCull(list<Entity*> *inEntities, list<Entity*> *outEntities);

	float m_worldLowerBound;
	int m_numSceneGraphCulledEntities;
	int m_numFrustumCulledEntities;
	int m_numEntitiesRendered;
};

#endif