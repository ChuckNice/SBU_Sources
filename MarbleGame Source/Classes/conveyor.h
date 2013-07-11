#ifndef __CONVEYOR_H__
#define __CONVEYOR_H__

#include "platform.h"

class Conveyor : public Platform
{
public:
	Conveyor(Scene *scene, Vector3 size, float speed);
	EntityType getType(){ return ET_CONVEYOR; }

	void onCreateRigidBody( btCollisionShape *sharedShape );
	void onCollision(CollidableEntity* collider);
    bool onInitialize(void);

    float getSpeed() { return m_rotSpeed; }
	Vector3 getOrientation(void){ return m_convOrientation; }

private:
	float m_rotSpeed;
	Vector3 m_convOrientation;
};

#endif