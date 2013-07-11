#ifndef __PLATFORM_H__
#define __PLATFORM_H__

#include "cube.h"

/**
This class represents a platform. A platform is just a special
cube that is static in the world (not a dynamic object but still collidable)
*/
class Platform : public Cube
{
public:
	Platform(Scene *scene, Vector3 size);
	virtual EntityType getType() { return ET_PLATFORM; }
	virtual void onCreateRigidBody(btCollisionShape *sharedShape);

};

#endif