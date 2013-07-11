#ifndef __BUTTON_H__
#define __BUTTON_H__

#include "collidable_entity.h"

#define INFINITE_REFRESH_TIME HUGE_VAL

typedef enum button_state {
	BS_PRESSED = 0,
	BS_UNPRESSED,
} ButtonState;

class Button : public CollidableEntity
{
public:
	Button(Scene *scene, Vector3 buttonSize, float refreshTime);

	// entity lifecycle
	bool onInitialize();
	void onPopulateVBOs();
	void onCreateRigidBody(btCollisionShape *sharedShape);
	void onPrepare(float dt);
	void onRender() const;
	void onPostRender();
	void onShutdown();
	void onCollision(CollidableEntity* collider);
	void registerForCollisionUpdates();
	EntityType getType() { return ET_BUTTON; }
	CullingType getCullingType() { return CT_AABB; }

	// getters
	Vector3 getSize() { return m_size; }

protected:
	// subclasses must implement this to handle being pressed
	virtual void onPressed() = 0;

	Vector3 m_size;
	float m_refreshTime;
	float m_cooldown;
	ButtonState m_currentState;

	float m_unpressedHeight;
};

#endif