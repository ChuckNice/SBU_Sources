#ifndef __CUBE_H__
#define __CUBE_H__

#include "collidable_entity.h"

/**
This class repsents a simple cube entity. Cubes are dynamic objects that are manipulated
by bullet3D. If you want a static cube, use the platform class instead.
*/
class Cube : public CollidableEntity
{
public:
	Cube(Scene *scene, Vector3 size);
	virtual EntityType getType() { return ET_CUBE; }
	CullingType getCullingType() { return CT_AABB; }

	// entity lifecycle
	bool onInitialize();
	void onPopulateVBOs();
	virtual void onCreateRigidBody(btCollisionShape *sharedShape);
	void onPrepare(float dt);
	void onRender() const;
	void onPostRender();
	void onShutdown();
	virtual void onCollision(CollidableEntity* collider);
    
    Vector3 getSize() { return m_size; }

protected:
	Vector3 m_size;
};

#endif