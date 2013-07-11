#include "sphere.h"

Sphere::Sphere(Scene *scene, float radius) :
CollidableEntity(scene),
	m_radius(radius)
{
}

bool Sphere::onInitialize()
{
	return true;
}

void Sphere::onPopulateVBOs()
{
	int stacks = 20;
	int slices = 20;


	float x = 0.0f, y = 0.0f, z = 0.0f, s = 0.0f, t = 0.0f;

	//Color col = Color(RANDF(1.0f), RANDF(1.0f), RANDF(1.0f));

	float PIOverStacks = kmPI / float(stacks);
	float PI2OverSlices = 2.0f * kmPI / float(slices);
	for (int stack = 0; stack < stacks; ++stack) 
	{
		unsigned int size = (unsigned int)m_vertices.size();

		float Phi = float(stack) * PIOverStacks;
		float CosP = cosf(Phi);
		float SinP = sinf(Phi);
		for (int slice = 0; slice < slices + 1; ++slice)
		{
			float Theta = float(slice) * PI2OverSlices;
			x = m_radius * cosf(Theta) * SinP;
			y = m_radius * sinf(Theta) * SinP;
			z = m_radius * CosP;
			s = 1.0f - (float)slice / (float)slices;
			t = (float)stack / (float)stacks;

			Vertex vertex = Vertex(x, y, z);
			m_vertices.push_back(vertex);
			m_colors.push_back(Color(fabs(s - 0.5f), fabs(t), 1.0f));
			//m_colors.push_back(col);
			m_texCoords.push_back(TexCoord(s, t));
			vertex.normalize();
			m_normals.push_back(vertex);

			float nextPhi = float(stack + 1) * PIOverStacks;
			float nextCosP = cosf(nextPhi);
			float nextSinP = sinf(nextPhi);
			x = m_radius * cosf(Theta) * nextSinP;
			y = m_radius * sinf(Theta) * nextSinP;
			z = m_radius * nextCosP;
			s = 1.0f - (float)slice / (float)slices;
			t = (float)(stack + 1.0f) / (float)stacks;

			vertex = Vertex(x, y, z);
			m_vertices.push_back(vertex);
			m_colors.push_back(Color(fabs(s - 0.5f), fabs(t), 1.0f));
			//m_colors.push_back(col);
			m_texCoords.push_back(TexCoord(s, t));
			vertex.normalize();
			m_normals.push_back(vertex);
		}

		m_vertices.push_back(m_vertices[size]);
		m_colors.push_back(m_colors[size]);
		m_texCoords.push_back(m_texCoords[size]);
		m_normals.push_back(m_normals[size]);

		m_vertices.push_back(m_vertices[size+1]);
		m_colors.push_back(m_colors[size+1]);
		m_texCoords.push_back(m_texCoords[size+1]);
		m_normals.push_back(m_normals[size+1]);   
	}
}

void Sphere::onCreateRigidBody(btCollisionShape *sharedShape)
{
	// create shape
	btCollisionShape* shape;
	if (sharedShape == NULL)
		shape = new btSphereShape(btScalar(m_radius));
	else
		shape = sharedShape;

	// set initial transform
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(getModelviewMatrix().mat);

	// determine mass (proportional to size) and local inertia
	btScalar mass(m_radius * 2.0f);
	btVector3 localInertia(0,0,0);
	if (mass != 0.f)
		shape->calculateLocalInertia(mass, localInertia);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);
	
	// sphere must have non-zero friction to roll
	body->setFriction(1.0f);
	body->setRollingFriction(0.1f);
	body->setAnisotropicFriction(shape->getAnisotropicRollingFrictionDirection(), btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);

	//body->setLinearVelocity(btVector3(5.0f, 0.0f, -5.0f));

	// set this collidable entities rigid body
	m_rigidBody = body;
}

void Sphere::onPrepare(float dt)
{
}

void Sphere::onRender() const
{
}

void Sphere::onPostRender()
{
}

void Sphere::onShutdown()
{
}

void Sphere::onCollision(CollidableEntity* collider)
{
}