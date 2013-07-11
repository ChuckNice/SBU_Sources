#ifndef __CANNON_H__
#define __CANNON_H__

#include "cylinder.h"

class Cannon : public Cylinder
{
public:
	Cannon(Scene *scene, Vector3 barrelHalfExtents, float shootSpeed, float autoFire = -1.0f, float rotateSpeed = 0.0f);
	virtual EntityType getType() { return ET_CANNON; }

    void onPrepare(float dt);
	void shootCannon();
    
    void setFireTime(float fireTime) { m_autoFire = fireTime; }

private:
	float m_shootSpeed;
    float m_autoFire;
    float m_refreshTime;
	float m_rotateSpeed;
};

#endif