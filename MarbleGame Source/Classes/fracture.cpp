
#include "fracture.h"
#include "scene_graph.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"


Fracture::Fracture(Scene *scene, FractureObject *parent, float explodeSpeed) :
CollidableEntity(scene),
m_fractured(false),
m_parent(parent),
m_explodeSpeed(explodeSpeed)
{
    frac_vertices = &m_vertices;
    frac_colors = &m_colors;
    frac_texCoords = &m_texCoords;
    frac_normals = &m_normals;
}

bool Fracture::onInitialize()
{
    return true;
}

void Fracture::onPopulateVBOs()
{
    // NOTE: all the vertex arrays should have already been populated at this point by FractureObject
}

void Fracture::preCreateRigidBody()
{
    onCreateRigidBody(NULL);
}

void Fracture::onCreateRigidBody(btCollisionShape *sharedShape)
{
    if (m_rigidBody == NULL) {
        m_fractureDirection = Vector3(0.0f, 0.0f, 0.0f);
        int count = 0;
        
        // create dynamic triangle mesh shape
        btCollisionShape *shape;
        if (sharedShape == NULL) {
            btTriangleMesh *triangleMesh = new btTriangleMesh();
            for (unsigned int i = 0; i < m_vertices.size(); i+=3) {
                m_fractureDirection += m_vertices[i+0].btVec() + m_vertices[i+1].btVec() + m_vertices[i+2].btVec();
                triangleMesh->addTriangle(m_vertices[i+0].btVec(), m_vertices[i+1].btVec(), m_vertices[i+2].btVec());
                count++;
                
            }
            shape = new btConvexTriangleMeshShape(triangleMesh);
        }
        else
            shape = sharedShape;
        
        // calculate fracture direction which is average of all triangle position
        m_fractureDirection = m_fractureDirection * (1.0f / (float)count);
        m_fractureDirection.normalize();
        Matrix4 modelview;
        modelview.rotatef(m_yaw, 0.0f, -1.0f, 0.0f);
        modelview.rotatef(m_pitch, -1.0f, 0.0f, 0.0f);
        modelview.rotatef(m_roll, 0.0f, 0.0f, -1.0f);
        kmVec3Transform(&m_fractureDirection, &m_fractureDirection, &modelview);
        
        // set initial transform
        btTransform startTransform;
        startTransform.setFromOpenGLMatrix(getModelviewMatrix().mat);
        
        // NRTODO: mass for Fracture is wrong. should approximate size with AABB and use that for mass
        btScalar mass(2.0f);
        
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
}

void Fracture::fracture()
{
    if (!m_fractured) {
        getRigidBody()->setLinearVelocity((m_fractureDirection * m_explodeSpeed).btVec());
        m_fractured = true;
    }
}

void Fracture::onPrepare(float dt) { }
void Fracture::onRender() const { glDisable(GL_CULL_FACE); }
void Fracture::onPostRender() { glEnable(GL_CULL_FACE); }
void Fracture::onShutdown() { }
void Fracture::onCollision(CollidableEntity* collider) { }






FractureObject::FractureObject(CollidableEntity *entity, const string& fractureFile, float explodeSpeed) :
m_origEntity(entity),
m_fractureFile(fractureFile),
m_collisionGhost(NULL),
m_scene(NULL),
m_shouldFracture(false),
m_isFractured(false),
m_explodeSpeed(explodeSpeed)
{
    m_fractureMap = new map<long, Fracture*>();
}

FractureObject::~FractureObject()
{
    if (m_origEntity)
        delete m_origEntity;
    
    // need to NULL fracture shaders because shared shader was deleted when origEntity was deleted above
    for (FractureIterator it = m_fractureMap->begin(); it != m_fractureMap->end(); ++it)
    {
        Fracture *frac = (*it).second;
        frac->setShaderProgram(NULL);
    }
    
    delete m_fractureMap;
    
    if (m_collisionGhost != NULL) {
        m_scene->getDynamicsWorld()->removeCollisionObject(m_collisionGhost);
        delete m_collisionGhost->getCollisionShape();
        delete m_collisionGhost;
    }
}

bool FractureObject::addToScene(Scene *scene)
{
    m_scene = scene;
    
    if (!m_origEntity->isInitialized()) {
        GLOG("ERROR: need to initialize original object before passing to FractureObject\n");
        return false;
    }
    
    // load fracture image
    TargaImage image;
    if ( !image.load(m_fractureFile) ) {
        GLOG("Error: could not load fracture file\n");
        return false;
    }
    
    // create triangles out of the sphere points
    btTriangleMesh *triangleMesh = new btTriangleMesh();
    int stride = (m_origEntity->getRenderingMode() == GL_TRIANGLE_STRIP) ? 2 : 3;
    int offset = (m_origEntity->getRenderingMode() == GL_TRIANGLE_STRIP) ? 3 : 2;
    for (int i = 0; i < m_origEntity->getVertices()->size() - offset; i += stride)
    {
        int u = (int)(m_origEntity->getTexCoords()->at(i).s * image.getWidth());
        int v = (int)(m_origEntity->getTexCoords()->at(i).t * image.getHeight());
        int voronoi_index = (v * image.getWidth() + u) * image.getNumBytesPerPixel();
        unsigned char R = image.getImageData()[voronoi_index];
        unsigned char G = image.getImageData()[voronoi_index+1];
        unsigned char B = image.getImageData()[voronoi_index+2];
        long voronoi_hash = float(R) + (256.0f * float(G)) + (512.0f * float(B));
        
        if (m_fractureMap->find(voronoi_hash) == m_fractureMap->end())
            (*m_fractureMap)[voronoi_hash] = new Fracture(scene, this, m_explodeSpeed);
        (*m_fractureMap)[voronoi_hash]->frac_vertices->push_back(m_origEntity->getVertices()->at(i));
        (*m_fractureMap)[voronoi_hash]->frac_colors->push_back(m_origEntity->getColors()->at(i));
        (*m_fractureMap)[voronoi_hash]->frac_texCoords->push_back(m_origEntity->getTexCoords()->at(i));
        (*m_fractureMap)[voronoi_hash]->frac_normals->push_back(m_origEntity->getNormals()->at(i));
        (*m_fractureMap)[voronoi_hash]->frac_vertices->push_back(m_origEntity->getVertices()->at(i+1));
        (*m_fractureMap)[voronoi_hash]->frac_colors->push_back(m_origEntity->getColors()->at(i+1));
        (*m_fractureMap)[voronoi_hash]->frac_texCoords->push_back(m_origEntity->getTexCoords()->at(i+1));
        (*m_fractureMap)[voronoi_hash]->frac_normals->push_back(m_origEntity->getNormals()->at(i+1));
        (*m_fractureMap)[voronoi_hash]->frac_vertices->push_back(m_origEntity->getVertices()->at(i+2));
        (*m_fractureMap)[voronoi_hash]->frac_colors->push_back(m_origEntity->getColors()->at(i+2));
        (*m_fractureMap)[voronoi_hash]->frac_texCoords->push_back(m_origEntity->getTexCoords()->at(i+2));
        (*m_fractureMap)[voronoi_hash]->frac_normals->push_back(m_origEntity->getNormals()->at(i+2));
        triangleMesh->addTriangle(m_origEntity->getVertices()->at(i).btVec(),
                                  m_origEntity->getVertices()->at(i+1).btVec(),
                                  m_origEntity->getVertices()->at(i+2).btVec());
        
        if (m_origEntity->getRenderingMode() == GL_TRIANGLE_STRIP)
        {
            u = (int)(m_origEntity->getTexCoords()->at(i+1).s * image.getWidth());
            if (u == image.getWidth()) { u--; }
            v = (int)(m_origEntity->getTexCoords()->at(i+1).t * image.getHeight());
            if (v == image.getHeight()) { v--; }
            voronoi_index = (v * image.getWidth() + u) * image.getNumBytesPerPixel();
            R = image.getImageData()[voronoi_index];
            G = image.getImageData()[voronoi_index+1];
            B = image.getImageData()[voronoi_index+2];
            voronoi_hash = float(R) + (256.0f * float(G)) + (512.0f * float(B));
            
            if (m_fractureMap->find(voronoi_hash) == m_fractureMap->end())
                (*m_fractureMap)[voronoi_hash] = new Fracture(scene, this, m_explodeSpeed);
            (*m_fractureMap)[voronoi_hash]->frac_vertices->push_back(m_origEntity->getVertices()->at(i+2));
            (*m_fractureMap)[voronoi_hash]->frac_colors->push_back(m_origEntity->getColors()->at(i+2));
            (*m_fractureMap)[voronoi_hash]->frac_texCoords->push_back(m_origEntity->getTexCoords()->at(i+2));
            (*m_fractureMap)[voronoi_hash]->frac_normals->push_back(m_origEntity->getNormals()->at(i+2));
            (*m_fractureMap)[voronoi_hash]->frac_vertices->push_back(m_origEntity->getVertices()->at(i+1));
            (*m_fractureMap)[voronoi_hash]->frac_colors->push_back(m_origEntity->getColors()->at(i+1));
            (*m_fractureMap)[voronoi_hash]->frac_texCoords->push_back(m_origEntity->getTexCoords()->at(i+1));
            (*m_fractureMap)[voronoi_hash]->frac_normals->push_back(m_origEntity->getNormals()->at(i+1));
            (*m_fractureMap)[voronoi_hash]->frac_vertices->push_back(m_origEntity->getVertices()->at(i+3));
            (*m_fractureMap)[voronoi_hash]->frac_colors->push_back(m_origEntity->getColors()->at(i+3));
            (*m_fractureMap)[voronoi_hash]->frac_texCoords->push_back(m_origEntity->getTexCoords()->at(i+3));
            (*m_fractureMap)[voronoi_hash]->frac_normals->push_back(m_origEntity->getNormals()->at(i+3));
            triangleMesh->addTriangle(m_origEntity->getVertices()->at(i+2).btVec(),
                                      m_origEntity->getVertices()->at(i+1).btVec(),
                                      m_origEntity->getVertices()->at(i+3).btVec());
        }
    }
    
    // initialize and render all fractures of this object (don't add to dynamic world)
    int count = 0;
    for (FractureIterator it = m_fractureMap->begin(); it != m_fractureMap->end(); ++it)
    {
        Fracture *frac = (*it).second;
        if ( !frac->initialize(m_origEntity->getShaderProgram()) ) {
            GLOG("Could not initialize a fracture\n");
            return false;
        }
        frac->setPosition(m_origEntity->getPosition());
        frac->setOrientation(m_origEntity->getYaw(), m_origEntity->getPitch(), m_origEntity->getRoll());
        if (m_origEntity->getShaderProgram()->supportsTexuring())
            frac->loadTexture(m_origEntity->getTexture0File());
        frac->preCreateRigidBody();
        scene->getGraph()->attach(frac);
        count++;
    }
    
    // create the ghost object for this body to handle collision with all unfracctured fractures
    m_collisionGhost = new btPairCachingGhostObject();
    btTransform worldTransform;
    worldTransform.setFromOpenGLMatrix(m_origEntity->getModelviewMatrix().mat);
    m_collisionGhost->setWorldTransform(worldTransform);
    m_collisionGhost->setCollisionShape(new btConvexTriangleMeshShape(triangleMesh));
    scene->getDynamicsWorld()->addCollisionObject(m_collisionGhost);
    scene->registerForSimulationTicks(this);
    
    return true;
}

void FractureObject::prepare(float dt)
{
    if (m_shouldFracture && !m_isFractured)
    {
        m_scene->getDynamicsWorld()->removeCollisionObject(m_collisionGhost);
        delete m_collisionGhost->getCollisionShape();
        delete m_collisionGhost;
        m_collisionGhost = NULL;
        
        // fracture all the fragments
        for (FractureIterator it = m_fractureMap->begin(); it != m_fractureMap->end(); ++it)
        {
            Fracture *frac = (*it).second;
            frac->addToDynamicWorld(m_scene->getDynamicsWorld());
            frac->fracture();
        }
        
        m_isFractured = true;
    }
}

void FractureObject::simulationTick(float dt)
{
    if (m_collisionGhost) {
        // iterate through all collisions
        for (int i = 0; i < m_collisionGhost->getNumOverlappingObjects(); i++)
        {
            btCollisionObject *collideObj = m_collisionGhost->getOverlappingObject(i);
            CollidableEntity *collideEntity = (CollidableEntity*)collideObj->getUserPointer();
            if (collideEntity && collideEntity->getType() == ET_SPHERE)
            {
                fracture();
            }
        }
    }
}







