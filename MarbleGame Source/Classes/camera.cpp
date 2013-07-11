#include "camera.h"
#include "entity.h"
#include "frustum.h"
#include <list>

// singleton getter for camera
Camera* s_Cam()
{
	static Camera s_cam;
	return &s_cam;
}

Camera::Camera():
m_position(Vector3(-10.0f, 10.0f, 10.0f)),
	m_lookAt(Vector3(0.0f, 0.0f, 0.0f)),
	m_up(Vector3(0.0f, 1.0f, 0.0f)),
	m_attachedEntity(NULL),
	m_yaw(0.0f),
	m_pitch(0.0f),
	m_frustum(NULL),
m_screenWidth(600),
m_screenHeight(800)
{
	m_cameraMode = CM_3RD_PERSON;

	// calculate yaw and pitch from initial position and lookAt
	Vector3 forward = m_lookAt - m_position;
	forward.normalize();
	m_yaw = -acosf(forward.x) * kmPIUnder180;
	m_pitch = asinf(forward.y) * kmPIUnder180;

	m_frustum = new Frustum();
}

float elapsedTime = 0.0f; 
void Camera::update(float dt)
{
	// change camera mode is user presses C key
	if (s_Keyboard()->isKeyPressed('C')) {
		if (m_cameraMode == CM_3RD_PERSON) { m_cameraMode = CM_FREE_LOOK; }
		else { m_cameraMode = CM_3RD_PERSON; }
	}

	// handle free look mode
	if (m_cameraMode == CM_FREE_LOOK) 
	{
		// move with keys
		if (s_Keyboard()->isKeyHeldDown('W')) { moveForward(CAMERA_MOVE_SPEED * dt); }
		if (s_Keyboard()->isKeyHeldDown('S')) { moveForward(-CAMERA_MOVE_SPEED * dt); }
		if (s_Keyboard()->isKeyHeldDown('E')) { m_position.y += CAMERA_MOVE_SPEED * dt; }
		if (s_Keyboard()->isKeyHeldDown('Q')) { m_position.y += -CAMERA_MOVE_SPEED * dt; }

		// rotate with mouse (if not the first frame)
		int m_relX, m_relY;
		s_Mouse()->getRelativeMousePos(m_relX, m_relY);
		yaw(float(m_relX) * 40.0f * dt);
		pitch(float(m_relY) * -40.0f * dt);

		// calculate look at for new values
		m_lookAt = calculateLookAt(m_position, m_yaw, m_pitch);
	}
	// handle 3rd person mode
	else if (m_cameraMode == CM_3RD_PERSON)
	{
        if (m_attachedEntity != NULL) {
            // get attributes from attached entity
            m_position = m_attachedEntity->getPosition();
            m_yaw = m_attachedEntity->getYaw();
            m_pitch = m_attachedEntity->getPitch();
            
            // set lookAt to look at the entity
            m_lookAt = m_attachedEntity->getPosition();
            
            // calculate forward vector relative to position
            Vector3 look = calculateLookAt(m_position, m_yaw, m_pitch);
            Vector3 forward = look - m_position;
            forward.normalize();
            
            // move camera back and up a bit
            m_position = m_position - (forward * 15.0f);
            m_position += (m_up * 10.0f);
        }
	}
    else if (m_cameraMode == CM_ORBIT)
    {
        if (m_attachedEntity != NULL) {            
            Vector3 forward = m_lookAt - m_position;
            forward.normalize();
            Vector3 right = forward.cross(m_up);
            
            // move to the right in orbit around entity
            m_position += right * 10.0f * dt;
        }
    }
    
	// set the camera
	kmMat4LookAt(&m_modelviewMatrix, &m_position, &m_lookAt, &m_up);

	// calculate MVP matrixs
	kmMat4Multiply(&m_modelviewProjectionMatrix, &m_projectionMatrix, &m_modelviewMatrix);

	// update the frustum
	m_frustum->updateFrustum();
}

Vector3 Camera::calculateLookAt(Vector3 pos, float yaw, float pitch)
{
	float yaw_r = degToRad(yaw);
	float pitch_r = degToRad(pitch);

	// NOTE: this function isn't correct. the cosYaw and sinYaw should be multiplied by cosPitch (like in calculateLookDir).
	// However, doing it like this seems to make the camera move a little nicer (can never look straight up or down though).
	Vector3 look;
	float cosYaw = cosf(yaw_r);
	float sinYaw = sinf(yaw_r);
	float sinPitch = sinf(pitch_r);
	look.x = pos.x + cosYaw;
	look.y = pos.y + sinPitch;
	look.z = pos.z + sinYaw;

	return look;
}

Vector3 Camera::calculateLookDir(float yaw, float pitch)
{
	float yaw_r = degToRad(yaw);
	float pitch_r = degToRad(pitch);

	float cosYaw = cosf(yaw_r);
	float sinYaw = sinf(yaw_r);
	float sinPitch = sinf(pitch_r);
	float cosPitch = cosf(pitch_r);
	Vector3 look = Vector3(cosYaw * cosPitch, sinPitch, sinYaw * cosPitch);
	look.normalize();

	return look;
}

void Camera::yaw(const float val)
{
	m_yaw += val;

	// constrain yaw
	if (m_yaw >= 360.0f) m_yaw -= 360.0f;
	if (m_yaw < 0.0f) m_yaw += 360.0f;
}

void Camera::pitch(const float val)
{
	m_pitch += val;

	// constrain pitch
	const float PITCH_LIMIT = 90.0f;
	if (m_pitch >= PITCH_LIMIT) {
		m_pitch = PITCH_LIMIT;
	}
	else if (m_pitch <= -PITCH_LIMIT) {
		m_pitch = -PITCH_LIMIT;
	}

}

void Camera::moveForward(const float speed)
{
	Vector3 pos = m_position;

	// calculate direction we should move in, and move forward
	float cosYaw = cosf(degToRad(m_yaw));
	float sinYaw = sinf(degToRad(m_yaw));
	float sinPitch = sinf(degToRad(m_pitch));
	pos.x += cosYaw * speed;
	pos.y += sinPitch * speed;
	pos.z += sinYaw * speed;

	m_position = pos;
}


void Camera::onResize(int width, int height)
{
	glViewport(0, 0, width, height);
    m_screenWidth = width;
    m_screenHeight = height;
	m_screenRatio = float(m_screenWidth) / float(m_screenHeight);

	kmMat4PerspectiveProjection(&m_projectionMatrix, CAM_FOV, m_screenRatio, CAM_NEAR, CAM_FAR);
	kmMat4OrthographicProjection(&m_orthoProjectionMatrix, 0.0f, float(width), 0.0f, float(height), -1.0f, 1.0f);
}




