#include "cylinder.h"

Cylinder::Cylinder(Scene *scene, Vector3 halfExtents) :
CollidableEntity(scene),
	m_halfExtents(halfExtents)
{
	// size cannot be 0 in any dimension
	if (halfExtents.x <= 0) { halfExtents.x = 0.01f; }
	if (halfExtents.y <= 0) { halfExtents.y = 0.01f; }
	if (halfExtents.z <= 0) { halfExtents.z = 0.01f; }
}

bool Cylinder::onInitialize()
{
	return true;
}

void Cylinder::onPopulateVBOs()
{
	const int numSamples = 20;

	// store cylinder vertices into a temporary buffer
	Vector3 tempVertices[2][numSamples];
	Vector3 tempNormals[2][numSamples];
	float heights[2] = { -m_halfExtents.z, m_halfExtents.z };
	for (int i = 0; i < 2; i++) {
		float degreesDelta = 360.0f / float(numSamples);
		for (int j = 0; j < numSamples - 1; j++) {
			float degrees = (degreesDelta * float(j));
			Vector3 dir = s_Cam()->calculateLookDir(degrees, 0.0f);
			dir = Vector3(dir.x, dir.z, dir.y); // flip Y and Z
			tempNormals[i][j] = dir;
			dir.x *= m_halfExtents.x;
			dir.y *= m_halfExtents.y;
			Vector3 tempVertex = Vector3(dir.x, dir.y, heights[i]);
			tempVertices[i][j] = tempVertex;
		}
		tempVertices[i][numSamples-1] = tempVertices[i][0];
		tempNormals[i][numSamples-1] = tempNormals[i][0];
	}

	// push back vertices in triangle order
	for (int i = 0; i < numSamples - 1; i++) {
		m_vertices.push_back(tempVertices[0][i]);
		m_vertices.push_back(tempVertices[1][i]);
		m_vertices.push_back(tempVertices[1][i+1]);
		m_vertices.push_back(tempVertices[0][i]);
		m_vertices.push_back(tempVertices[1][i+1]);
		m_vertices.push_back(tempVertices[0][i+1]);
	}

	// push back colors
	for (int i = 0; i < numSamples - 1; i++) {
		m_colors.push_back(Color(0.1f, 0.8f, 0.2f));
		m_colors.push_back(Color(0.1f, 0.8f, 0.2f));
		m_colors.push_back(Color(0.1f, 0.8f, 0.2f));
		m_colors.push_back(Color(0.1f, 0.8f, 0.2f));
		m_colors.push_back(Color(0.1f, 0.8f, 0.2f));
		m_colors.push_back(Color(0.1f, 0.8f, 0.2f));
	}

	// push back normals
	for (int i = 0; i < numSamples - 1; i++) {
		m_normals.push_back(tempNormals[0][i]);
		m_normals.push_back(tempNormals[1][i]);
		m_normals.push_back(tempNormals[1][i+1]);
		m_normals.push_back(tempNormals[0][i]);
		m_normals.push_back(tempNormals[1][i+1]);
		m_normals.push_back(tempNormals[0][i+1]);
	}
}

void Cylinder::onCreateRigidBody(btCollisionShape *sharedShape)
{
	// create shape
	btCollisionShape* shape;
	if (sharedShape == NULL) {
		// NRTODO: the halfExtents I use to create cylinder vertices don't match what bullet uses. So collision is wrong for cylinders
		shape = new btCylinderShape(m_halfExtents.btVec());
	}
	else
		shape = sharedShape;

	// set initial transform
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(getModelviewMatrix().mat);

	// determine mass (proportional to size) and local inertia
	//btScalar mass(m_halfExtents.x * m_halfExtents.y * m_halfExtents.z);
	btScalar mass(0.0f);
	btVector3 localInertia(0,0,0);
	if (mass != 0.f)
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// set this collidable entities rigid body
	m_rigidBody = body;
}

void Cylinder::onPrepare(float dt)
{
}

void Cylinder::onRender() const
{
	glDisable(GL_CULL_FACE);
}

void Cylinder::onPostRender()
{
	glEnable(GL_CULL_FACE);
}

void Cylinder::onShutdown()
{
}

void Cylinder::onCollision(CollidableEntity* collider)
{
}