#include "collectible.h"
#include "player.h"

static float SPIN_SPEED = 7.0f;

Collectible::Collectible(Scene *scene, string objFile, int pointValue, float scale, bool useAABBApproximation, bool isDynamic, bool isFaceCulled):
OBJmodel(scene, objFile, scale, useAABBApproximation, isDynamic, isFaceCulled),
	m_isCollected(false),
	m_collectedValue(pointValue)
{
}

bool Collectible::onInitialize()
{
	if(!OBJmodel::onInitialize())
		return false;
	else
		return true;	
}

void Collectible::onCreateRigidBody( btCollisionShape *sharedShape )
{
	OBJmodel::onCreateRigidBody(NULL);
    registerForCollisionUpdates(NULL);
}

void Collectible::onPrepare( float dt )
{
	//update yaw
	setYaw(getYaw() + (SPIN_SPEED * dt));
	
	//get bullet representation
	btMotionState *ms = getRigidBody()->getMotionState();
	btTransform transform = getRigidBody()->getWorldTransform();

	//update bullet representation
	btQuaternion q = btQuaternion(getYaw(), getPitch(), 0.0f);
	transform *= btTransform(q);
	ms->setWorldTransform(transform);
}

void Collectible::onCollision(CollidableEntity* collider)
{
	// button is pressed if player collides and it is currently unpressed
	if (collider != NULL && collider->getType() == ET_PLAYER && !m_isCollected)
	{
		m_scene->getPlayer()->addToScore(m_collectedValue);
		m_isCollected = true;
		destroy();
	}
}

//void Collectible::onSimulationTick(float dt)
//{
//	setYaw(getYaw() + (SPIN_SPEED * dt));
//}