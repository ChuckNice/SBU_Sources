#include "button.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

Button::Button(Scene *scene, Vector3 size, float refreshTime) :
CollidableEntity(scene),
	m_size(size),
	m_refreshTime(refreshTime),
	m_cooldown(refreshTime),
	m_currentState(BS_UNPRESSED)
{
	// size cannot be 0 in any dimension
	if (size.x <= 0) { m_size.x = 0.01f; }
	if (size.y <= 0) { m_size.y = 0.01f; }
	if (size.z <= 0) { m_size.z = 0.01f; }
}

bool Button::onInitialize()
{
	return true;
}

void Button::onPopulateVBOs()
{
	// setup vertices
	const float width = m_size.x / 2.0f;
	const float height = m_size.y / 2.0f;
	const float depth = m_size.z / 2.0f;

	/// bottom
	m_vertices.push_back(Vertex( width, -height, -depth));
	m_vertices.push_back(Vertex( width, -height,  depth));
	m_vertices.push_back(Vertex(-width, -height, -depth));
	m_vertices.push_back(Vertex( width, -height,  depth));
	m_vertices.push_back(Vertex(-width, -height,  depth));
	m_vertices.push_back(Vertex(-width, -height, -depth));

	// top
	m_vertices.push_back(Vertex(-width, height, -depth));
	m_vertices.push_back(Vertex(-width, height,  depth));
	m_vertices.push_back(Vertex( width, height, -depth));
	m_vertices.push_back(Vertex(-width, height,  depth));
	m_vertices.push_back(Vertex( width, height,  depth));
	m_vertices.push_back(Vertex( width, height, -depth));

	// left
	m_vertices.push_back(Vertex(-width, -height,  depth));
	m_vertices.push_back(Vertex(-width, height,  depth));
	m_vertices.push_back(Vertex(-width, -height, -depth));
	m_vertices.push_back(Vertex(-width, height,  depth));
	m_vertices.push_back(Vertex(-width, height, -depth));
	m_vertices.push_back(Vertex(-width, -height, -depth));

	// right
	m_vertices.push_back(Vertex(width, -height, -depth));
	m_vertices.push_back(Vertex( width, height, -depth));
	m_vertices.push_back(Vertex(width, -height,  depth));
	m_vertices.push_back(Vertex( width, height, -depth));
	m_vertices.push_back(Vertex( width, height,  depth));
	m_vertices.push_back(Vertex(width, -height,  depth));

	// back
	m_vertices.push_back(Vertex( -width, -height, -depth));
	m_vertices.push_back(Vertex(-width, height, -depth));
	m_vertices.push_back(Vertex(width, -height, -depth));
	m_vertices.push_back(Vertex( -width, height, -depth));
	m_vertices.push_back(Vertex(width, height, -depth));
	m_vertices.push_back(Vertex(width, -height, -depth));

	// front
	m_vertices.push_back(Vertex(width, -height,  depth));
	m_vertices.push_back(Vertex( width, height,  depth));
	m_vertices.push_back(Vertex(-width, -height,  depth));
	m_vertices.push_back(Vertex(width, height,  depth));
	m_vertices.push_back(Vertex(-width, height,  depth));
	m_vertices.push_back(Vertex( -width, -height,  depth));


	// setup colors
	Color col = Color(RANDF(1.0f), RANDF(1.0f), RANDF(1.0f));
	for (int i = 0; i < 12; ++i) { m_colors.push_back(col); }
	col = Color(RANDF(1.0f), RANDF(1.0f), RANDF(1.0f));
	for (int i = 0; i < 12; ++i) { m_colors.push_back(col); }
	col = Color(RANDF(1.0f), RANDF(1.0f), RANDF(1.0f));
	for (int i = 0; i < 12; ++i) { m_colors.push_back(col); }


	// setup tex coords
	for (int i = 0; i < 6; i++) {
		m_texCoords.push_back(TexCoord(1.0f, 0.0f));
		m_texCoords.push_back(TexCoord(1.0f, 1.0f));
		m_texCoords.push_back(TexCoord(0.0f, 0.0f));
		m_texCoords.push_back(TexCoord(1.0f, 1.0f));
		m_texCoords.push_back(TexCoord(0.0f, 1.0f));
		m_texCoords.push_back(TexCoord(0.0f, 0.0f));
	}


	// setup normals
	for (int i = 0; i < 6; i++) { m_normals.push_back(Vertex(0, -1,  0)); }
	for (int i = 0; i < 6; i++) { m_normals.push_back(Vertex(0, 1,  0)); }
	for (int i = 0; i < 6; i++) { m_normals.push_back(Vertex(-1, 0,  0)); }
	for (int i = 0; i < 6; i++) { m_normals.push_back(Vertex(1, 0,  0)); }
	for (int i = 0; i < 6; i++) { m_normals.push_back(Vertex(0, 0,  -1)); }
	for (int i = 0; i < 6; i++) { m_normals.push_back(Vertex(0, 0,  1)); }
}

void Button::onCreateRigidBody(btCollisionShape *sharedShape)
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

	// determine mass (proportional to size) and local inertia
	btScalar mass(0.0f);
	btVector3 localInertia(0,0,0);
	if (mass != 0.f)
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	body->setFriction(1.0f);
	body->setRollingFriction(1.0f);

	// set this collidable entities rigid body
	m_rigidBody = body;

	// manually register for collision updates
	CollidableEntity::registerForCollisionUpdates();

	// store the height of the unpressed button
	m_unpressedHeight = m_position.y;
}

void Button::registerForCollisionUpdates()
{
	GLOG("WARNING: no need to call registerForCollisionUpdates() on button. button has built in support for collision detection\n");
	return;
}

void Button::onPrepare(float dt)
{
	if (m_collisionGhost)
	{
		btTransform currentTrans = getRigidBody()->getWorldTransform();
		Vector3 currentOrigin = Vector3(currentTrans.getOrigin());
		// move the button toward just sticking out of the floor is pressed
		if (m_currentState == BS_PRESSED) {
			if (currentOrigin.y > m_unpressedHeight - m_size.y + 0.1f)
				currentOrigin.y -= dt;
		}
		// move button back toward original position if it's unpressed
		else {
			if (currentOrigin.y < m_unpressedHeight)
				currentOrigin.y += dt;
		}
		// set the new transform
		currentTrans.setOrigin(currentOrigin.btVec());
		getRigidBody()->setWorldTransform(currentTrans);
		getRigidBody()->getMotionState()->setWorldTransform(currentTrans);
	}

	// if button cool down period is over, set the button to unpressed
	m_cooldown -= dt;
	if (m_cooldown <= 0.0f && m_currentState == BS_PRESSED)
		m_currentState = BS_UNPRESSED;
}

void Button::onRender() const
{
}

void Button::onPostRender()
{
}

void Button::onShutdown()
{
}

void Button::onCollision(CollidableEntity* collider)
{
	// button is pressed if player collides and it is currently unpressed
	if (collider != NULL && collider->getType() == ET_PLAYER && m_currentState == BS_UNPRESSED)
	{
		onPressed();
		m_currentState = BS_PRESSED;

		// button cannot be hit again until cooldown reaches 0
		m_cooldown = m_refreshTime;
	}
}