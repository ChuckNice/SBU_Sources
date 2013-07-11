#ifndef CAMERA_H_INCLUDED
#define CAMERA_H_INCLUDED

#include "global.h"

class Entity;
class Frustum;

#define CAMERA_MOVE_SPEED 50.0f
#define CAM_FOV 45.0f
#define CAM_NEAR 0.1f
#define CAM_FAR 2000.0f

typedef enum cam_mode {
	CM_3RD_PERSON,
	CM_FREE_LOOK,
    CM_STILL,
    CM_ORBIT,
} CamMode;


/**
This class is a singleton that manages the camera. The camera contains
several important matrices: modelview, projection, othographic projection,
modelviewprojection, and the player's modelviewprojection (for use in free look mode).
It also tracks the cameras mode.
*/
class Camera 
{
public:
    Camera();

	void update(float dt);
	void onResize(int width, int height);

	// getters
	Mat4 *modelviewMatrix() { return &m_modelviewMatrix; }
	Mat4 *projectionMatrix() { return &m_projectionMatrix; }
	Mat4 *modelviewProjectionMatrix() { return &m_modelviewProjectionMatrix; }
	Mat4 *orthoProjectionMatrix() { return &m_orthoProjectionMatrix; }

    void attachTo(Entity* entity) {  m_attachedEntity = entity; }
	Entity* getAttachedEntity() { return m_attachedEntity; }

	CamMode getCameraMode() { return m_cameraMode; }
    float getScreenWidth() { return m_screenWidth; }
    float getScreenHeight() { return m_screenHeight; }
	float getScreenRatio() { return m_screenRatio; }
	Frustum* getFrustum() { return m_frustum; }
	Vector3 getPosition() { return m_position; }
	Vector3 getLookAt() { return m_lookAt; }
	float getYaw() { return m_yaw; }
	float getPitch() { return m_pitch; }
    
    void setPosition(Vector3 pos) { m_position = pos; }
    void setLookAt(Vector3 look) { m_lookAt = look; }
    void setCameraMode(CamMode mode) { m_cameraMode = mode; }

	Vector3 calculateLookDir(float yaw, float pitch);

private:
	// camera movement functions
	void handleFreeLookMode(float dt);
    void yaw(const float degrees);
    void pitch(const float degrees);
	void moveForward(const float speed);
	Vector3 calculateLookAt(Vector3 pos, float yaw, float pitch);

private:
    Vector3 m_position;
	Vector3 m_lookAt;
    Vector3 m_up;

    float m_yaw;
    float m_pitch;
    
    float m_screenWidth;
    float m_screenHeight;
	float m_screenRatio;

	Mat4 m_modelviewMatrix;
	Mat4 m_projectionMatrix;
	Mat4 m_modelviewProjectionMatrix;
	Mat4 m_orthoProjectionMatrix;

    Entity* m_attachedEntity;
	CamMode m_cameraMode;

	Frustum *m_frustum;
};

// reference to singleton getter
extern Camera* s_Cam();

#endif // CAMERA_H_INCLUDED
