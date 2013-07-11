#pragma once
#ifndef __COLLECTIBLE_H__
#define __COLLECTIBLE_H__

#include "OBJmodel.h"

class Collectible :
	public OBJmodel
{
public:
	Collectible(Scene *scene, 
		string objFile, 
		int pointValue,
		float scale = 1.0f,
		bool useAABBApproximation = false, 
		bool isDynamic = false, 
		bool isFaceCulled = true
		);
	EntityType getType(){ return ET_COLLECTIBLE; }
    
	bool onInitialize();
	void onCreateRigidBody( btCollisionShape *sharedShape );
	void onCollision(CollidableEntity* collider);
	void onPrepare( float dt );
	//void onSimulationTick(float dt);

private:
	bool m_isCollected;
	int m_collectedValue;
};

#endif
