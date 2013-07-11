#include "cube.h"

Cube::Cube(Scene *scene, Vector3 size) :
CollidableEntity(scene),
	m_size(size)
{
	// size cannot be 0 in any dimension
	if (size.x <= 0) { m_size.x = 0.01f; }
	if (size.y <= 0) { m_size.y = 0.01f; }
	if (size.z <= 0) { m_size.z = 0.01f; }
}

bool Cube::onInitialize()
{
	return true;
}

void Cube::onPopulateVBOs()
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
	Color col = Color(RANDF(1.0f), RANDF(1.0f), RANDF(1.0f), 0.95f);
	for (int i = 0; i < 12; ++i) { m_colors.push_back(col); }
	//col = Color(RANDF(1.0f), RANDF(1.0f), RANDF(1.0f));
	for (int i = 0; i < 12; ++i) { m_colors.push_back(col); }
	//col = Color(RANDF(1.0f), RANDF(1.0f), RANDF(1.0f));
	for (int i = 0; i < 12; ++i) { m_colors.push_back(col); }


	// setup tex coords
	for (int i = 0; i < 6; i++) {
		float textureSize = 1.0f;
		m_texCoords.push_back(TexCoord(textureSize, 0.0f));
		m_texCoords.push_back(TexCoord(textureSize, textureSize));
		m_texCoords.push_back(TexCoord(0.0f, 0.0f));
		m_texCoords.push_back(TexCoord(textureSize, textureSize));
		m_texCoords.push_back(TexCoord(0.0f, textureSize));
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

void Cube::onCreateRigidBody(btCollisionShape *sharedShape)
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
	btScalar mass(m_size.x * m_size.y * m_size.z);
	btVector3 localInertia(0,0,0);
	if (mass != 0.f)
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	/*body->setFriction(1.0f);
	body->setRollingFriction(1.0f);*/

	// set this collidable entities rigid body
	m_rigidBody = body;
}

void Cube::onPrepare(float dt)
{
}

void Cube::onRender() const
{
}

void Cube::onPostRender()
{
}

void Cube::onShutdown()
{
}

void Cube::onCollision(CollidableEntity* collider)
{
}