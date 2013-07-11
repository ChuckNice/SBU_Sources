#include "trampoline.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

Trampoline::Trampoline(Scene *scene, float width, float height) :
CollidableEntity(scene),
	m_width(width),
	m_height(height),
	m_amplitude(0.0f),
	m_dipPercent(0.0f),
	m_maxDip(2.0f),
	m_timeout(0.0f),
	m_contactPoint(Vector3(0.0f, 0.0f, 0.0f))
{
	// size cannot be 0 in any dimension
	if (width <= 0.0f || height <= 0.0f) {
		GLOG("ERROR: tried to create trampoline with no width or height\n");
		throw;
	}
}

bool Trampoline::onInitialize()
{
	return true;
}

void Trampoline::onPopulateVBOs()
{
	// compute the min and max points
	m_maxX = m_width / 2.0f;
	m_maxZ = m_height / 2.0f;
	m_minX = -m_maxX;
	m_minZ = -m_maxZ;

	// generate trampoline in triangle strips
	for (int j = 0 ; j < m_height; j++) {
		for (int i = 0; i < m_width; i++) {
			pushVertexDataForIndices(i, j);
			pushVertexDataForIndices(i, j + 1);
			pushVertexDataForIndices(i + 1, j + 1);

			pushVertexDataForIndices(i, j);
			pushVertexDataForIndices(i + 1, j + 1);
			pushVertexDataForIndices(i + 1, j);			
		}
	}

	// send some stuff to shader
	ShaderProgram->bindShader();
	ShaderProgram->sendUniform("MaxPoint", m_maxX, 0.0f, m_maxZ);
}

void Trampoline::pushVertexDataForIndices(int i, int j)
{
	Vector3 vertex = Vector3(m_minX + i, 0.0f, m_minZ + j);
	m_vertices.push_back(vertex);
	m_texCoords.push_back(TexCoord((float(i) / m_width) * 2.0f, (float(j) / m_height) * 2.0f));
	float x_dist = max(fabsf(vertex.x) - 0.2f, 0.0f);
	float z_dist = max(fabsf(vertex.z) - 0.2f, 0.0f);
	m_colors.push_back(Color(x_dist / m_maxX, 0.0f, z_dist / m_maxZ));
	m_normals.push_back(Normal(0.0f, 1.0f, 0.0f));

}

void Trampoline::onCreateRigidBody(btCollisionShape *sharedShape)
{
	// create shape
	const float width = m_width / 2.0f;
	const float height = 0.5f;
	const float depth = m_height / 2.0f;
	btCollisionShape* shape;
	if (sharedShape == NULL)
		shape = new btBoxShape(btVector3(btScalar(width),btScalar(height),btScalar(depth)));
	else
		shape = sharedShape;

	// set initial transform
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(getModelviewMatrix().mat);

	// trampoline is a static object
	btScalar mass(0);
	btVector3 localInertia(0,0,0);

	//using motionstate is recommended, it provides interpolation capabilities, and only synchronizes 'active' objects
	btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, shape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	body->setFriction(1.0f);
	body->setRollingFriction(1.0f);

	// set this collidable entities rigid body
	m_rigidBody = body;


	// create the ghosht object for this body
	m_ghostObject = new btPairCachingGhostObject();
	m_ghostObject->setWorldTransform(startTransform);
	m_ghostObject->setCollisionShape (shape);
	m_ghostObject->setCollisionFlags(m_ghostObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	m_dynamicWorld->addCollisionObject(m_ghostObject);

	// calculate the worldspace normal for later use
	Vector3 normal = Vector3(0.0f, 1.0f, 0.0f);
    const Matrix4& modelview = getModelviewMatrix();
	kmVec3TransformNormal(&m_WCnormal, &normal, &modelview);
	m_WCnormal.normalize();
}

void Trampoline::onPrepare(float dt)
{
	if (m_dipPercent > 0.0f)
		m_dipPercent -= 2.0f * dt;
}

void Trampoline::onRender() const
{
	ShaderProgram->sendUniform("ContactPoint", m_contactPoint.x, m_contactPoint.y, m_contactPoint.z);
	ShaderProgram->sendUniform("Amplitude", m_amplitude);
	ShaderProgram->sendUniform("DipPercent", m_dipPercent);

	glDisable(GL_CULL_FACE);
}

void Trampoline::onPostRender()
{
	glEnable(GL_CULL_FACE);
}

const float timeout_threshold = 0.05f;
const float speed_for_max_dip = 30.0f;
void Trampoline::onSimulationTick(float dt)
{
	if (m_timeout <= timeout_threshold)
		m_timeout += dt;

	// get the array of overlapping pairs from the ghost object
	btBroadphasePairArray& pairArray = m_ghostObject->getOverlappingPairCache()->getOverlappingPairArray();

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
			const btCollisionObject *thisObj = (manifold->getBody0() == m_ghostObject) ? manifold->getBody0() : manifold->getBody1();
			const btCollisionObject *otherObj = (thisObj == manifold->getBody0()) ? manifold->getBody1() : manifold->getBody0();

			// only consider collision if it's not between the ghost object and the rigid body of this entity...
			if (thisObj && otherObj &&
				otherObj != getRigidBody() &&
				manifold->getNumContacts() > 0) 
			{
				btRigidBody *otherBody = (btRigidBody*)btRigidBody::upcast(otherObj);
				if (otherBody) {
					CollidableEntity *otherEntity = (CollidableEntity*)otherBody->getUserPointer();
					//btRigidBody *otherBody = (btRigidBody*)manifold->getBody1()->getUserPointer();

					// we only need one contact point on the trampoline...
					const btManifoldPoint& pt = manifold->getContactPoint(0);
					if (pt.getDistance() <= 0.1f) 
					{
						if (m_timeout > timeout_threshold) {
							// get point of contact on trampoline (in world space)
							const Vector3 worldPointOnTrampoline = Vector3(pt.getPositionWorldOnA());

							// get the point of contact in model space
							Vector3 modelPointOnTrampoline;
							Matrix4 inverseModelview;
                            const Matrix4& modelview = getModelviewMatrix();
							kmMat4Inverse(&inverseModelview, &modelview);
							kmVec3Transform(&modelPointOnTrampoline, &worldPointOnTrampoline, &inverseModelview);

							// get incoming velocity of the object (previous frame) and reflect it over plane normal
							Vector3 incomingVel = otherEntity->getPreviousVelocity();
							Vector3 reflectedVel = incomingVel - (m_WCnormal * (incomingVel.dot(m_WCnormal) * 2.0f));

							// set the velocity to the reflected velocity across the normal
							otherBody->setLinearVelocity(reflectedVel.btVec());
							m_timeout = 0.0f;

							// set the contact point and amplitude based on incoming velocity
							m_contactPoint = Vector3(modelPointOnTrampoline.x, 0.0f, modelPointOnTrampoline.z); // we only care about x,z for contact point
							float speed = incomingVel.length();
							float ratio = min(speed / speed_for_max_dip, 1.0f);
							m_amplitude = m_maxDip * ratio;
							m_dipPercent = 1.0f;
						}
					}
				}
			}
		}
	}
}

void Trampoline::onShutdown()
{
}

void Trampoline::onCollision(CollidableEntity* collider)
{
}