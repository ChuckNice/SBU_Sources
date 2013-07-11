#ifndef __CYLINDER_H__
#define __CYLINDER_H__

#include "collidable_entity.h"

class Cylinder : public CollidableEntity
{
public:
	Cylinder(Scene *scene, Vector3 halfExtents);
	virtual EntityType getType() { return ET_CYLINDER; }
	CullingType getCullingType() { return CT_RADIUS; }

	// entity lifecycle
	virtual bool onInitialize();
	void onPopulateVBOs();
	void onCreateRigidBody(btCollisionShape *sharedShape);
	virtual void onPrepare(float dt);
	void onRender() const;
	void onPostRender();
	void onShutdown();
	void onCollision(CollidableEntity* collider);

protected:
	Vector3 m_halfExtents;
};

#endif