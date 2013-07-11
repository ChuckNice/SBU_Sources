#include "conveyor.h"


Conveyor::Conveyor(Scene *scene, Vector3 size, float speed) :
Platform(scene, size),
m_rotSpeed(speed),
m_convOrientation(Vector3(0.0f, 0.0f, 0.0f))
{
	
}

bool Conveyor::onInitialize(void)
{
	m_convOrientation = s_Cam()->calculateLookDir(m_yaw + 90, m_pitch);
	return true;
}

void Conveyor::onCollision(CollidableEntity* collider)
{

	if( collider != NULL && collider->getType() == ET_PLAYER ){

		//utility function that calculates the normalized "direction/orientation" of the entity
		m_convOrientation = s_Cam()->calculateLookDir(m_yaw + 90, m_pitch);
		collider->getRigidBody()->applyCentralImpulse(m_convOrientation.btVec() * m_rotSpeed);
	}
}


void Conveyor::onCreateRigidBody( btCollisionShape *sharedShape )
{	
	Platform::onCreateRigidBody(sharedShape);

	//not quite the effect I wanted
	//getRigidBody()->setRollingFriction(0.1f);
	//getRigidBody()->setFriction(2.0f);

	registerForCollisionUpdates();
}
