#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "collidable_entity.h"

/**
This class represents a sphere. Spheres are collidable dynamics objects that
roll on surfaces with a non-zero frictional coefficient.
*/
class Sphere : public CollidableEntity
{
public:
	Sphere(Scene *scene, float radius);
	virtual EntityType getType() { return ET_SPHERE; }
	CullingType getCullingType() { return CT_AABB; }

	// entity lifecycle
	bool onInitialize();
	void onPopulateVBOs();
	virtual void onCreateRigidBody(btCollisionShape *sharedShape);
	virtual void onPrepare(float dt);
	void onRender() const;
	virtual void onPostRender();
	virtual void onShutdown();
	virtual void onCollision(CollidableEntity* collider);

protected:
	float m_radius;

	GLenum getRenderingMode() { return GL_TRIANGLE_STRIP; }
};

#endif