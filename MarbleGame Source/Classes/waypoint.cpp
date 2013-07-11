#include "waypoint.h"
#include "player.h"
#include "particles.h"

Waypoint::Waypoint(Scene *scene, string objFile, float scale, bool useAABBApproximation, bool isDynamic, bool isFaceCulled):
OBJmodel(scene, objFile, scale, useAABBApproximation, isDynamic, isFaceCulled),
	m_particleSys(NULL)
{
}

bool Waypoint::onInitialize()
{
	if(!OBJmodel::onInitialize())
		return false;
	else
		return true;	
}

void Waypoint::onCreateRigidBody( btCollisionShape *sharedShape )
{
	OBJmodel::onCreateRigidBody(NULL);
    registerForCollisionUpdates(new btBoxShape(btVector3(0.01f, 0.01f, 0.01f)));
}

void Waypoint::onCollision(CollidableEntity* collider)
{
	//// button is pressed if player collides and it is currently unpressed
	//if (collider != NULL && collider->getType() == ET_PLAYER)
	//{
	//	m_scene->resetPlayer();
	//}
}

void Waypoint::onPrepare( float dt )
{

}

void Waypoint::attachParticleSystem(ParticleSystem *system)
{
	m_particleSys = system; 
	system->moveToPoint(getPosition().x, getPosition().y, getPosition().z);
}