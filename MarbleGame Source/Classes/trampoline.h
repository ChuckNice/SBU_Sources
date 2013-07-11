#ifndef __TRAMPOLINE_H__
#define __TRAMPOLINE_H__

#include "collidable_entity.h"

class btPairCachingGhostObject;

class Trampoline : public CollidableEntity
{
public:
	Trampoline(Scene *scene, float width, float height);
	virtual EntityType getType() { return ET_TRAMPOLINE; }
	CullingType getCullingType() { return CT_AABB; }

	// entity lifecycle
	bool onInitialize();
	void onPopulateVBOs();
	void onCreateRigidBody(btCollisionShape *sharedShape);
	void onPrepare(float dt);
	void onRender() const;
	void onPostRender();
	void onShutdown();
	void onCollision(CollidableEntity* collider);

	void onSimulationTick(float dt);

private:
	float m_width;
	float m_height;
	float m_minX, m_minZ;
	float m_maxX, m_maxZ;
	Vector3 m_WCnormal;

	Vector3 m_contactPoint;
	float m_amplitude;
	float m_dipPercent;
	float m_maxDip;
	float m_timeout;

	void pushVertexDataForIndices(int i, int j);

	// ghost object for performing collision queries
	btPairCachingGhostObject *m_ghostObject;
};

#endif