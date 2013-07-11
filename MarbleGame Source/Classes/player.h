#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "sphere.h"

class Waypoint;
/**
This class represents the player. The player is a sphere that the camera will
following in 3rd person. It is moved with the W,A,S,D keys and will jump when
spacebar is hit. The camera is moved around the player with the mouse.
*/
class Player : public Sphere
{
public:
	Player(Scene *scene, float radius);
	virtual EntityType getType() { return ET_PLAYER; }

	void onCreateRigidBody(btCollisionShape *sharedShape);
	void onPrepare(float dt);
	void onSimulationTick(float dt);
	void onPostRender();
	void onShutdown();
    void onCollision(CollidableEntity *collider);

	void setGlowColor(const Color& color) { m_glowColor = color; }
	void addToScore(int Score);
	int getScore(void) {return m_score;}
	void setCheckpoint(Waypoint *checkpoint){ m_WPlastReached = checkpoint; }
	Waypoint* getLastCheckpoint(void) { return m_WPlastReached; }

private:
	Shader *m_glowShader;
	Color m_glowColor;
	GLuint m_glowVertexVBO;
	GLuint m_glowTexCoordVBO;

	void yaw(const float val);
	void pitch(const float val);
	void moveForward(const float speed);
    
    bool m_canJump;
    float m_jumpRefresh;
	int m_score;
	Waypoint *m_WPlastReached;
};

#ifdef MARBLE_IOS_BUILD
extern Vector3 getiOSMovementVector();
#endif

#endif