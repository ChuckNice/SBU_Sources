#ifndef __COLLIDABLE_ENTITY__
#define __COLLIDABLE_ENTITY__

#include "entity.h"
#include "btBulletDynamicsCommon.h"

class btPairCachingGhostObject;

class CollidableEntity : public Entity
{
public:
	CollidableEntity(Scene *scene);
	~CollidableEntity();

	void addToDynamicWorld(btDynamicsWorld *dynamicWorld, btCollisionShape *sharedShape = NULL);
	
	// entity lifecycle
	void prepare(float dt);
	void simulationTick(float dt);
	virtual EntityType getType() = 0;
	virtual CullingType getCullingType() = 0;
	
	// call this method to have the onCollision method get called during collisions
	virtual void registerForCollisionUpdates(btCollisionShape *custumShape = NULL);

	Vector3 getPreviousVelocity() { return m_prevVelocity; }

protected:
	// subclass lifecycle
	virtual bool onInitialize() = 0;
	virtual void onPopulateVBOs() = 0;
	virtual void onCreateRigidBody(btCollisionShape *sharedShape) = 0;
	virtual void onPrepare(float dt) = 0;
	virtual void onRender() const = 0;
	virtual void onPostRender() = 0;
	virtual void onShutdown() = 0;
	
	// this is only called if call the registerForCollisionUpdates
	virtual void onCollision(CollidableEntity* collider) = 0;

	btDynamicsWorld *m_dynamicWorld;
	Vector3 m_prevVelocity;

	// ghost object for performing collision queries
	btPairCachingGhostObject *m_collisionGhost;
};

#endif