#include "scene_graph.h"
#include "btBulletDynamicsCommon.h"
#include "frustum.h"
#include "player.h"
#include "particles.h"
#include "waypoint.h"

SceneGraph::SceneGraph(Scene *scene) :
m_scene(scene),
	m_numSceneGraphCulledEntities(0),
	m_numFrustumCulledEntities(0),
	m_numEntitiesRendered(0),
	m_worldLowerBound(DEFAULT_WOLRD_LOWER_BOUND)
{
	// manually initialize the root node and set it's bounds
	m_rootNode = new GraphNode(NULL);
	m_rootNode->boundingBox.minVertex = Vector3(-ROOT_NODE_BOUNDS, -ROOT_NODE_BOUNDS, -ROOT_NODE_BOUNDS);
	m_rootNode->boundingBox.maxVertex = Vector3(ROOT_NODE_BOUNDS, ROOT_NODE_BOUNDS, ROOT_NODE_BOUNDS);

	// m_currentNode is root node to start
	m_currentNode = m_rootNode;
}

SceneGraph::~SceneGraph()
{
	// delete all entities
	for (EntityIterator entity = m_entities.begin(); entity != m_entities.end(); ++entity) {
		(*entity)->shutdown();
		delete (*entity);
	}
    
    // delete all particle systems
	for (ParticleSystemIterator system = m_particleSystems.begin(); system != m_particleSystems.end(); ++system)
		delete (*system);

	delete m_rootNode;
}


// attaches an entity to the scene graph. Scene graph will manage memory of object from now on
void SceneGraph::attach(Entity *entity)
{
	// if the entity has an AABB, add it's aabb to the current graph node
	if (entity->getRigidBody() != NULL) {
		btVector3 minPoint, maxPoint;
		entity->getRigidBody()->getAabb(minPoint, maxPoint);
		AABB aabb = AABB(Vertex(minPoint), Vertex(maxPoint));
		m_currentNode->boundingBox.addAABB(aabb, 1.0f);
		entity->setGraphNode(m_currentNode);
	}

	m_entities.push_back(entity);
}

// detaches an entity from the scene graph. entity memmory will be deleted
void SceneGraph::detach(Entity *entity)
{
	entity->destroy();
}


// attaches a particle system to the scene graph
void SceneGraph::attach(ParticleSystem *system)
{
	m_particleSystems.push_back(system);
}


// pushes a new node into the scene graph by creating new m_currentNode from parent
void SceneGraph::pushNode()
{
	m_currentNode = new GraphNode(m_currentNode);
}

// pops current node by adding as a child to it's parent node setting m_currentNode to parent
void SceneGraph::popNode()
{
	// don't let user pop root node
	if (m_currentNode == m_rootNode) {
		GLOG("WARNING: tried to pop the root node of the scene graph");
		return;
	}

	// add current node to it's parent and set current node to parent
	m_currentNode->parent->addChild(m_currentNode);
	m_currentNode = m_currentNode->parent;
}



void SceneGraph::update(float dt)
{
	// prepare entities for rendering
	for (EntityIterator entity = m_entities.begin(); entity != m_entities.end();)
	{
		Entity *e = (*entity);
        
		// if enemy is dead, clean it up and delete it
		if ( e->canBeRemoved() ) {
			e->shutdown();
			delete e;
			
			entity = m_entities.erase(entity);
			continue;
		}

		e->prepare(dt);

		// if an entity has left the bounds of the world, it should be removed
		if ( e->getPosition().y < m_worldLowerBound && e->getType() != ET_PLAYER) {
            e->destroy();
            ++entity;
			continue;
		}

		// test if the entity has left it's graph node. if so, attach it to parent
		if (e->getGraphNode() != NULL && e->getGraphNode() != m_rootNode && e->getRigidBody() != NULL) 
		{
			btVector3 minPoint, maxPoint;
			e->getRigidBody()->getAabb(minPoint, maxPoint);
			AABB aabb = AABB(Vertex(minPoint), Vertex(maxPoint));

			// if entity isn't fully insides graph nodes AABB, it has left the node (set to parent node)
			if ( !aabb.fullyInsideAABB(e->getGraphNode()->boundingBox)) {
				e->setGraphNode(e->getGraphNode()->parent);
			}
		}

		// manually increment the iterator
		++entity;
	}
    
    // update particle systems
	for (ParticleSystemIterator system = m_particleSystems.begin(); system != m_particleSystems.end(); ++system)
		(*system)->update(dt);
    
    // need to check player leaving bounds separately so we can move the 
	// rigidBody, the motionstate, and the player position
    if ( m_scene->getPlayer() && m_scene->getPlayer()->getPosition().y < m_worldLowerBound){
		
		Player* pplayer = m_scene->getPlayer();

		// If at least one checkpoint has been reached, true if Start object was added to the level.
		if(pplayer->getLastCheckpoint())
		{
			btMotionState *playerMS = pplayer->getRigidBody()->getMotionState();

			// move the players motionState to the last reached check point
			btTransform transform;
			transform = pplayer->getLastCheckpoint()->getRigidBody()->getWorldTransform();

			// add to the origin to ensure the player is not placed inside the waypoint
			transform.setOrigin(transform.getOrigin() + btVector3(0.0f, 0.5f, 0.0f));
			playerMS->setWorldTransform(transform);

			// get the players new transform from the motionstate
			playerMS->getWorldTransform(transform);
			// move the rigidBody to correspond
			pplayer->getRigidBody()->setWorldTransform(transform);
			// move the player to correspond
			pplayer->setPosition(transform.getOrigin());
		}else{
			GLOG("ERROR: no Checkpoints have been reached. Did You add a Start waypoint?\n");
            m_scene->resetPlayer();
			//throw;
		}
	}
}

void SceneGraph::render()
{
	// scene graph cull the entities
	list<Entity*> sceneGraphCulledEntities;
	sceneGraphCull(&m_entities, &sceneGraphCulledEntities);
	m_numSceneGraphCulledEntities = (int)sceneGraphCulledEntities.size();

	// frustum cull the entities that have been scene graph culled
	list<Entity*> frustumCulledEntities;
	frustumCull(&sceneGraphCulledEntities, &frustumCulledEntities);
	m_numFrustumCulledEntities = (int)frustumCulledEntities.size();

	m_numEntitiesRendered = 0;
	
	// render entities
	for (EntityIterator entity = frustumCulledEntities.begin(); entity != frustumCulledEntities.end(); ++entity)
	{
		(*entity)->render();
		(*entity)->postRender();
		m_numEntitiesRendered++;
	}
    
    // render particle systems last (for blending)
	for (ParticleSystemIterator system = m_particleSystems.begin(); system != m_particleSystems.end(); ++system)
		(*system)->render();
}


// recursive funtion to mark a node as unseen
void markNodeAsUnseen(GraphNode *node) {
	node->canBeSeen = false;
	for (unsigned int i = 0; i < node->children.size(); i++)
		markNodeAsUnseen(node->children[i]);
}

// recursive funtion to test and set if node can be seen
void testAndSetNodeVisibility(GraphNode *node) {
	if (s_Cam()->getFrustum()->isAABBInFrustum(&node->boundingBox)) {
		node->canBeSeen = true;
		for (unsigned int i = 0; i < node->children.size(); i++)
			testAndSetNodeVisibility(node->children[i]);
	}
}

void SceneGraph::sceneGraphCull(list<Entity*> *inEntities, list<Entity*> *outEntities)
{
	// safety check that graph has been popped to root node before rendering
	if (m_currentNode != m_rootNode) {
		GLOG("WARNING: popNode was not called for every pushNode when creating scene. Cannot perform scene graph culling");

		// since we can't scene graph cull, just pass along entities for now
		for (EntityIterator entity = inEntities->begin(); entity != inEntities->end(); ++entity)
			outEntities->push_back((*entity));

		return;
	}

	// mark all graph as unseen recursively
	markNodeAsUnseen(m_rootNode);

	// test each node recursively and set if it's visible
	testAndSetNodeVisibility(m_rootNode);

	// iterate through all entities
	for (EntityIterator entity = inEntities->begin(); entity != inEntities->end(); ++entity) {
		Entity *e = (*entity);

		// if entity does not have graph node, just pass it along
		if (e->getGraphNode() == NULL) {
			outEntities->push_back((*entity));
            continue;
        }

		// if entities graph node can be seen, then it passes scene graph culling
		if (e->getGraphNode()->canBeSeen)
			outEntities->push_back((*entity));
	}
}

void SceneGraph::frustumCull(list<Entity*> *inEntities, list<Entity*> *outEntities)
{
	// iterate through all incoming entities
	for (EntityIterator entity = inEntities->begin(); entity != inEntities->end(); ++entity)
	{
		Entity *e = (*entity);

		// only frustum cull entities with a rigid body
		// NRTODO: Entity should provide an AABB so we can frustum cull non-CollidableEntity objects
		if ( !e->getRigidBody() ) {
			outEntities->push_back((*entity));
			continue;
		}

		// get the AABB of the entity
		btVector3 minPoint, maxPoint;
		e->getRigidBody()->getAabb(minPoint, maxPoint);
		AABB aabb = AABB(Vertex(minPoint), Vertex(maxPoint));

		// cull the entity based on it's culling type
		if (e->getCullingType() == CT_AABB) {
			if (s_Cam()->getFrustum()->isAABBInFrustum(&aabb)) {
				outEntities->push_back((*entity));
				continue;
			}
		}
		else if (e->getCullingType() == CT_RADIUS) {
			if (s_Cam()->getFrustum()->isSphereInFrustum(e->getPosition().x, e->getPosition().y, e->getPosition().z, aabb.getRadius())) {
				outEntities->push_back((*entity));
				continue;
			}
		}
	}
}