#pragma once
#ifndef __WAYPOINT_H__
#define __WAYPOINT_H__

#include "OBJmodel.h"

typedef enum WP_Type {
	WP_START = 0,
	WP_FINISH,
	WP_CHECKPOINT,
	TOTAL_WP_TYPES
} WaypointType;

class ParticleSystem;
class Waypoint :
	public OBJmodel
{
public:
	Waypoint(Scene *scene, string objFile, float scale, bool useAABBApproximation, bool isDynamic, bool isFaceCulled);
	EntityType getType(void){ return ET_WAYPOINT; }
    
	bool onInitialize();
	void onCreateRigidBody( btCollisionShape *sharedShape );
	virtual void onPrepare( float dt );
	virtual void onCollision(CollidableEntity* collider);
	virtual WP_Type getWaypointType(void) = 0;
	virtual void attachParticleSystem( ParticleSystem *system);

protected:
	ParticleSystem *m_particleSys;
};

#endif
