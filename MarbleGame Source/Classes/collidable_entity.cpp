#include "collidable_entity.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

CollidableEntity::CollidableEntity(Scene *scene) :
Entity(scene),
	m_dynamicWorld(NULL),
	m_collisionGhost(NULL)
{
}

CollidableEntity::~CollidableEntity()
{
	if (getRigidBody() != NULL)
	{
		btRigidBody *body = getRigidBody();
		if (body->getCollisionShape()) { delete getRigidBody()->getCollisionShape(); }
		if (body->getMotionState()) { delete getRigidBody()->getMotionState(); }
		if (m_dynamicWorld != NULL) { m_dynamicWorld->removeCollisionObject(body); }
		delete body;
	}

	if (m_collisionGhost != NULL)
	{
		if (m_dynamicWorld != NULL) { m_dynamicWorld->removeCollisionObject(m_collisionGhost); }
		delete m_collisionGhost;
	}
}

void CollidableEntity::addToDynamicWorld(btDynamicsWorld *dynamicWorld, btCollisionShape *sharedShape)
{
	m_dynamicWorld = dynamicWorld;

	// have the subclass create the body
	if ( !getRigidBody() ) {
		onCreateRigidBody(sharedShape);
	}

	if ( getRigidBody() ) {
		// set the user pointer to this object (we use this in colllision detection)
		getRigidBody()->setUserPointer(this);

		// add the rigid body to the dynamic world
		dynamicWorld->addRigidBody(getRigidBody());
	}
}

void CollidableEntity::registerForCollisionUpdates(btCollisionShape *custumShape)
{
	if (m_dynamicWorld == NULL) {
		GLOG("ERROR: registerForCollisionUpdates MUST be called after addToDynamicWorld\n");
		throw;
	}

	if (!m_collisionGhost)
	{
		// create the ghost object for this body
		m_collisionGhost = new btPairCachingGhostObject();
        m_collisionGhost->setWorldTransform(getRigidBody()->getWorldTransform());
        if (custumShape == NULL)
            m_collisionGhost->setCollisionShape(getRigidBody()->getCollisionShape());
        else
            m_collisionGhost->setCollisionShape(custumShape);
		m_collisionGhost->setCollisionFlags(m_collisionGhost->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		m_dynamicWorld->addCollisionObject(m_collisionGhost);
        
        m_scene->registerForSimulationTicks(this);
	}
}

void CollidableEntity::prepare(float dt)
{
	// store the previous velocity
	if (getRigidBody())
		m_prevVelocity = getRigidBody()->getLinearVelocity();

	// cascade the call
	Entity::prepare(dt);
}

void CollidableEntity::simulationTick(float dt)
{
	// update the ghost object if we have one for collision detection
	if (m_collisionGhost) {
		m_collisionGhost->setWorldTransform(getRigidBody()->getWorldTransform());

		int numCollisions = m_collisionGhost->getNumOverlappingObjects();

		// pass NULL to onCollision when no entities are colliding
		if (numCollisions == 0)
			onCollision(NULL);

		// call onCollision for all overlapping entities that aren't this one
		for (int i = 0; i < numCollisions; i++) 
		{
			btCollisionObject *collideObj = m_collisionGhost->getOverlappingObject(i);
			CollidableEntity *collideEntity = (CollidableEntity*)collideObj->getUserPointer();
			if (collideEntity != this) {
				onCollision(collideEntity);
			}
		}
	}

	// cascade the call
	Entity::simulationTick(dt);
}