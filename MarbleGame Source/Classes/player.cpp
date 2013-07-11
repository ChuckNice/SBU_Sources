#include "player.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"


Player::Player(Scene *scene, float radius) :
Sphere(scene, radius),
	m_glowVertexVBO(0),
	m_glowTexCoordVBO(0),
	m_glowShader(NULL),
	m_glowColor(1.0f, 1.0f, 1.0f, 1.0f),
    m_canJump(false),
    m_jumpRefresh(-1.0f),
	m_score(0),
	m_WPlastReached(NULL)
{
}

void Player::onCreateRigidBody(btCollisionShape *sharedShape)
{
	Sphere::onCreateRigidBody(sharedShape);

	getRigidBody()->setActivationState(DISABLE_DEACTIVATION);
	getRigidBody()->setFriction(1.0f);
	getRigidBody()->setRollingFriction(0.8f);
    
    registerForCollisionUpdates();
}

const float player_move_speed = 200.0f;
const float player_max_speed = 20.0f;
const float player_jump_speed = 30.0f;

void Player::onPrepare(float dt)
{
	// move the ball with WASD keys
#ifndef MARBLE_IOS_BUILD
	if (s_Cam()->getCameraMode() != CM_FREE_LOOK && s_Keyboard()->isKeyHeldDown('W')) {
		Vector3 look = s_Cam()->calculateLookDir(m_yaw, 0.0f);
		Vector3 force = (look * player_move_speed * dt);
		getRigidBody()->applyCentralImpulse(btVector3(force.x, force.y, force.z));
	}
	if (s_Cam()->getCameraMode() != CM_FREE_LOOK && s_Keyboard()->isKeyHeldDown('S')) {
		Vector3 look = s_Cam()->calculateLookDir(m_yaw, 0.0f);
		Vector3 force = (look * -player_move_speed * dt);
		getRigidBody()->applyCentralImpulse(btVector3(force.x, force.y, force.z));
	}
	if (s_Cam()->getCameraMode() != CM_FREE_LOOK && s_Keyboard()->isKeyHeldDown('D')) {
		Vector3 look = s_Cam()->calculateLookDir(m_yaw, 0.0f);
		Vector3 right = look.cross(Vector3(0.0f, 1.0f, 0.0f));
		Vector3 force = (right * player_move_speed * dt);
		getRigidBody()->applyCentralImpulse(btVector3(force.x, force.y, force.z));
	}
	if (s_Cam()->getCameraMode() != CM_FREE_LOOK && s_Keyboard()->isKeyHeldDown('A')) {
		Vector3 look = s_Cam()->calculateLookDir(m_yaw, 0.0f);
		Vector3 right = look.cross(Vector3(0.0f, 1.0f, 0.0f));
		Vector3 force = (right * -player_move_speed * dt);
		getRigidBody()->applyCentralImpulse(btVector3(force.x, force.y, force.z));
	}
#else
    Vector3 movementDir = getiOSMovementVector();
    if (movementDir.x != 0.0f || movementDir.y != 0.0f) {
        float length = movementDir.length();
        float ratio = min(length / 30.0f, 1.0f);
        movementDir.normalize();
        Vector3 look = s_Cam()->calculateLookDir(m_yaw, 0.0f);
		Vector3 right = look.cross(Vector3(0.0f, 1.0f, 0.0f));
        Vector3 normalized_look = look * -movementDir.y;
        Vector3 normalized_right = right * movementDir.x;
        Vector3 force = (normalized_look + normalized_right) * player_move_speed * 0.5f * ratio * dt;
        getRigidBody()->applyCentralImpulse(btVector3(force.x, force.y, force.z));
    }
#endif
    
    // decrement jump refresh
    if (m_jumpRefresh > 0.0f)
        m_jumpRefresh -= dt;

	// jump with spacebar
	if (s_Cam()->getCameraMode() != CM_FREE_LOOK && (s_Keyboard()->isKeyHeldDown(Keyboard::KC_SPACE) || s_Mouse()->isButtonPressed(0)) &&
        m_canJump && m_jumpRefresh <= 0.0f) {
		Vector3 up = Vector3(0.0f, 1.0f, 0.0f);
		Vector3 force = (up * player_jump_speed);
		getRigidBody()->applyCentralImpulse(btVector3(force.x, force.y, force.z));
        m_jumpRefresh = 0.2f;
	}

	// rotate yaw based on mouse
	int m_relX, m_relY;
	s_Mouse()->getRelativeMousePos(m_relX, m_relY);
	if ( s_Cam()->getCameraMode() != CM_FREE_LOOK ) { 
		yaw(float(m_relX) * 40.0f * dt); 
		pitch(float(-m_relY) * 40.0f * dt); 
	}
}

void Player::onSimulationTick(float dt)
{
	// limit the velocity of the ball
	Vector3 velocity = Vector3(getRigidBody()->getLinearVelocity());
	Vector3 cappedVel = velocity;
	float speed = velocity.length();
	if (speed > player_max_speed)
		cappedVel = velocity * (player_max_speed / speed);
	getRigidBody()->setLinearVelocity(btVector3(cappedVel.x, velocity.y, cappedVel.z));
    
    
    m_canJump = false;
    
    // get the array of overlapping pairs from the ghost object
	btBroadphasePairArray& pairArray = m_collisionGhost->getOverlappingPairCache()->getOverlappingPairArray();
    
	// iterate through all the pairs, generating a manifold of contact points
	btManifoldArray manifoldArray;
	for (int i = 0; i < pairArray.size(); i++)
	{
		const btBroadphasePair& pair = pairArray[i];
		manifoldArray.clear();
        
		// unless we manually perform collision detection on this pair, the contacts are in the dynamics world paircache:
		btBroadphasePair* collisionPair = m_dynamicWorld->getPairCache()->findPair(pair.m_pProxy0,pair.m_pProxy1);
		if (!collisionPair)
			continue;
        
		// parse the collision into an array of contact manifolds
		if (collisionPair->m_algorithm)
			collisionPair->m_algorithm->getAllContactManifolds(manifoldArray);
        
		// we only need the first contact manifold to get a contact point...
		for (int j = 0; j < manifoldArray.size(); j++)
		{
			btPersistentManifold* manifold = manifoldArray[j];
			const btCollisionObject *thisObj = (manifold->getBody0() == m_collisionGhost) ? manifold->getBody0() : manifold->getBody1();
			const btCollisionObject *otherObj = (thisObj == manifold->getBody0()) ? manifold->getBody1() : manifold->getBody0();
            
			// only consider collision if it's not between the ghost object and the rigid body of this entity...
			if (thisObj && otherObj &&
				otherObj != getRigidBody() &&
				manifold->getNumContacts() > 0)
			{
				btRigidBody *otherBody = (btRigidBody*)btRigidBody::upcast(otherObj);
				if (otherBody) {
					//CollidableEntity *otherEntity = (CollidableEntity*)otherBody->getUserPointer();

					// we only need one contact point on the trampoline...
					const btManifoldPoint& pt = manifold->getContactPoint(0);
					if (pt.getDistance() <= 0.1f)
					{
						m_canJump = true;
					}
				}
			}
		}
	}
}

void Player::onPostRender()
{
	Sphere::onPostRender();

	// generate vertices of glow is we haven't
	/*if (m_glowShader == NULL) {
		m_glowShader = new GlowShader(m_glowColor);

		// send vertices
		float glowRadius = m_radius + 0.15f;
		Vector3 glowVertices[4] = {
			Vector3(-glowRadius, -glowRadius, 0.0f),
			Vector3(-glowRadius,  glowRadius, 0.0f),
			Vector3( glowRadius,  glowRadius, 0.0f),
			Vector3( glowRadius, -glowRadius, 0.0f)
		};
		glGenBuffers(1, &m_glowVertexVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_glowVertexVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 4, &glowVertices[0], GL_STATIC_DRAW);

		// send tex coords
		TexCoord glowTexCoords[4] = {
			TexCoord(0.0f, 0.0f),
			TexCoord(0.0f, 1.0f),
			TexCoord(1.0f, 1.0f),
			TexCoord(1.0f, 0.0f)
		};
		glGenBuffers(1, &m_glowTexCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_glowTexCoordVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 4, &glowTexCoords[0], GL_STATIC_DRAW);
	}

	m_glowShader->bindShader();

	// send vertices to shader
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_glowVertexVBO);
    glVertexAttribPointer((GLint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// send texcoords to shader
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, m_glowTexCoordVBO);
    glVertexAttribPointer((GLint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);

	// calculate the modelview for a billboarded sprite at players position
	Matrix4 modelview = *s_Cam()->modelviewMatrix();
	Vector3 lookDir = m_position - s_Cam()->getPosition();
	lookDir.normalize();
	Vector3 glowPosition = m_position - (lookDir * (m_radius + 0.25f));
	modelview.translatef(glowPosition.x, glowPosition.y, glowPosition.z);
	modelview.rotatef(s_Cam()->getYaw() - 90.0f, 0.0f, -1.0f, 0.0f);
	modelview.rotatef(s_Cam()->getPitch(), -1.0f, 0.0f, 0.0f);
	modelview.scalef(1.0f);

	// send matrices to shader program
	m_glowShader->sendUniform4x4("modelview_matrix", modelview.mat);
	m_glowShader->sendUniform4x4("projection_matrix", s_Cam()->projectionMatrix()->mat);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);

	glDrawArrays(GL_QUADS, 0, 4);

	// cleanup
	glDisable(GL_BLEND);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);
	glUseProgram(0);*/
}

void Player::onShutdown()
{
	if (m_glowShader != NULL) {
		glDeleteBuffers(1, &m_glowVertexVBO);
		glDeleteBuffers(1, &m_glowTexCoordVBO);
		delete m_glowShader;
	}
	Sphere::onShutdown();
}

void Player::yaw(const float val)
{
	m_yaw += val;

	// constrain yaw
	if (m_yaw >= 360.0f) m_yaw -= 360.0f;
	if (m_yaw < 0.0f) m_yaw += 360.0f;
}

void Player::pitch(const float val)
{
	m_pitch += val;

	// constrain pitch
	const float PITCH_LIMIT = 30.0f;
	if (m_pitch >= PITCH_LIMIT) {
		m_pitch = PITCH_LIMIT;
	}
	else if (m_pitch <= -PITCH_LIMIT) {
		m_pitch = -PITCH_LIMIT;
	}

}

void Player::moveForward(const float speed)
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


void Player::onCollision(CollidableEntity *collider) {
    //m_canJump = (collider != NULL &&
    //             (collider->getType() == ET_PLATFORM || collider->getType() == ET_OBJ_MODEL) );
}

void Player::addToScore(int score){
	m_score += score;
}