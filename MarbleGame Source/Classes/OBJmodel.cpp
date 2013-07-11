#include "OBJmodel.h"

OBJmodel::OBJmodel(Scene *scene, 
	string objFile, 
	float scale, 
	bool useAABBApproximation, 
	bool isDynamic,
	bool isFaceCulled) : 
CollidableEntity(scene),
	m_objFile(objFile),
	m_size(Vector3(0.01f, 0.01f, 0.01f)),
	m_scale(scale),
	m_isDynamic(isDynamic),
	m_useAABBApproximation(useAABBApproximation),
	m_isFaceCulled(isFaceCulled)
{
}

bool OBJmodel::onInitialize()
{
	//temporary storage for object data
	vector<Vector3> tempVertices;
	vector<Normal> tempNormals;
	vector<TexCoord> tempTexCoords;

	//get contents of obj file
	FILE *file = fopen(m_objFile.c_str(), "r");
	if (!file) { GLOG("OBJ file could not be read"); return false; }

	// temps used during parsing
	char buf[128];
	Vector3 vec;
	TexCoord tex;
	int v, t, n;

	//parse the obj file
	while(fscanf(file, "%s", buf) != EOF) 
	{
		switch(buf[0]) {
		case 'v':               // v, vn, vt 
			switch(buf[1]) {
			case '\0':          // vertex
				fscanf(file, "%f %f %f", &vec.x, &vec.y, &vec.z);
				tempVertices.push_back(vec);
				break;
			case 'n':           // normal
				fscanf(file, "%f %f %f", &vec.x, &vec.y, &vec.z);
				tempNormals.push_back(vec);
				break;
			case 't':           // texcoord
				fscanf(file, "%f %f", &tex.s, &tex.t);
				tempTexCoords.push_back(tex);
				break;
			default:
				// eat up rest of line (blank or unknown or not used)
				fgets(buf, sizeof(buf), file);
				break;
			}
			break;
		case 'f':               // face
			v = t = n = 0;
			fscanf(file, "%s", buf);
			// can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d 
			if (strstr(buf, "//")) {
				// v//n
				sscanf(buf, "%d//%d", &v, &n);
				Vector3 vertex = tempVertices[v-1]; m_vertices.push_back(vertex);
				Normal normal = tempNormals[n-1]; m_normals.push_back(normal);
				while(fscanf(file, "%d//%d", &v, &n) > 0) {
					vertex = tempVertices[v-1]; m_vertices.push_back(vertex);
					normal = tempNormals[n-1]; m_normals.push_back(normal);
				}
			} else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3) {
				// v/t/n
				Vector3 vertex = tempVertices[v-1]; m_vertices.push_back(vertex);
				TexCoord texCoord = tempTexCoords[t-1]; m_texCoords.push_back(texCoord);
				Normal normal = tempNormals[n-1]; m_normals.push_back(normal);
				while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0) {
					vertex = tempVertices[v-1]; m_vertices.push_back(vertex);
					texCoord = tempTexCoords[t-1]; m_texCoords.push_back(texCoord);
					normal = tempNormals[n-1]; m_normals.push_back(normal);
				}
			} else if (sscanf(buf, "%d/%d", &v, &t) == 2) {
				// v/t
				Vector3 vertex = tempVertices[v-1]; m_vertices.push_back(vertex);
				TexCoord texCoord = tempTexCoords[t-1]; m_texCoords.push_back(texCoord);
				while(fscanf(file, "%d/%d", &v, &t) > 0) {
					vertex = tempVertices[v-1]; m_vertices.push_back(vertex);
					texCoord = tempTexCoords[t-1]; m_texCoords.push_back(texCoord);
				}
			} else {
				// v
				while(fscanf(file, "%d", &v) > 0) {
					Vector3 vertex = tempVertices[v-1]; m_vertices.push_back(vertex);
				}
			}
			break;
		default:
			// eat up rest of line (blank or unknown or not used)
			fgets(buf, sizeof(buf), file);
			break;
		}
	}

	// scale all the vertices by scaling vector
	for (unsigned int i = 0; i < m_vertices.size(); i++)
		m_vertices[i] = m_vertices[i] * m_scale;

	// generate an AABB for this model so we can get it's size
	AABB boundingBox;
	for (unsigned int i = 0; i < m_vertices.size(); i++)
		boundingBox.addPoint(m_vertices[i]);

	// normalize the model by shifting all vertices relative to the boundingBox center (this will center the model at (0, 0, 0) in our world)
	// by normalizing the model, we can set the position and orientation of the model like any other object in our game, regardless of models position in the modeling program
	Vector3 shiftVector = Vector3(0.0f, 0.0f, 0.0f) - (boundingBox.maxVertex + boundingBox.minVertex) * 0.5f;
	for (unsigned int i = 0; i < m_vertices.size(); i++)
		m_vertices[i] = m_vertices[i] + shiftVector;

	// get dimensions of this object from the bounding box
	m_size = boundingBox.maxVertex - boundingBox.minVertex;
	if (m_size.x <= 0) { m_size.x = 0.01f; }
	if (m_size.y <= 0) { m_size.y = 0.01f; }
	if (m_size.z <= 0) { m_size.z = 0.01f; }

	return true;
}

void OBJmodel::onPopulateVBOs()
{    

}

void OBJmodel::onCreateRigidBody( btCollisionShape *sharedShape )
{
	const float width = m_size.x / 2.0f;
	const float height = m_size.y / 2.0f;
	const float depth = m_size.z / 2.0f;

	// create shape
	btCollisionShape* shape;
	if (sharedShape == NULL) {
		if ( m_useAABBApproximation ) {
			// if the model is dynamic, we can't use a triangle mesh (bullet triangle mesh is onl static)
			// so, we approximate the shape with a bounding of the model size
			shape = new btBoxShape(btVector3(btScalar(width),btScalar(height),btScalar(depth)));
		}
		else {
			// build a triangle mesh for bullet
			btTriangleMesh *triangleMesh = new btTriangleMesh();
			for (unsigned int i = 0; i < m_vertices.size(); i+=3)
				triangleMesh->addTriangle(m_vertices[i+0].btVec(), m_vertices[i+1].btVec(), m_vertices[i+2].btVec());

			if ( m_isDynamic ) {
				// if model is dynamic, we use bullet's convex triangle mesh to have accurate
				//triangle collisions. This is VERY expensive though so only use for low poly-count models
				shape = new btConvexTriangleMeshShape(triangleMesh);
			}
			else {
				// if model is static, we use bullet's static triangle mesh to have accurate
				// triangle collisions without tremendous overhead
				shape = new btBvhTriangleMeshShape(triangleMesh, true);
			}
		}
	}
	else
		shape = sharedShape;

	// set initial transform
	btTransform startTransform;
	startTransform.setFromOpenGLMatrix(getModelviewMatrix().mat);

	// mass is proportional to size
	btScalar mass(0.f);
	if ( m_isDynamic)
		mass = (width * height * depth);

	// only calculate the local inertia is the object is dynamic
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

}

void OBJmodel::onPrepare( float dt )
{
}

void OBJmodel::onRender() const
{
	// if model isn't showing, you can uncomment this to check if it's because of face culling. 
	// Make sure to comment it again afterwards!
	if (!m_isFaceCulled)
		glDisable(GL_CULL_FACE);
}

void OBJmodel::onPostRender()
{
    if (!m_isFaceCulled)
        glEnable(GL_CULL_FACE);
}

void OBJmodel::onShutdown()
{
}

void OBJmodel::onCollision( CollidableEntity* collider )
{
}
