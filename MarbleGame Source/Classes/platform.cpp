#include "platform.h"

Platform::Platform(Scene *scene, Vector3 size) :
Cube(scene, size)
{
}

void Platform::onCreateRigidBody(btCollisionShape *sharedShape)
{
	// create shape
	const float width = m_size.x / 2.0f;
	const float height = m_size.y / 2.0f;
	const float depth = m_size.z / 2.0f;
	btCollisionShape* shape;
	if (sharedShape == NULL)
		shape = new btBoxShape(btVector3(btScalar(width),btScalar(height),btScalar(depth)));
	else
		shape = sharedShape;

	// set initial transform
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(getModelviewMatrix().mat);

	// mass and local inertia are 0 (platforms are static)
	btScalar mass(0.f);
	btVector3 localInertia(0,0,0);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// platform should have some friction
	body->setFriction(1.0f);
	body->setRollingFriction(1.0f);

	// set this collidable entities rigid body
	m_rigidBody = body;
}
