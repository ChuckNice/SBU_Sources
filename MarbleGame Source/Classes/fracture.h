
#ifndef __marblegame__fracture__
#define __marblegame__fracture__

#include "collidable_entity.h"

class FractureObject;

class Fracture : public CollidableEntity
{
public:
    Fracture(Scene *scene, FractureObject *parent, float explodeSpeed);
    
    virtual EntityType getType() { return ET_FRACTURE; }
	CullingType getCullingType() { return CT_AABB; }
    bool usesSharedShader() { return true; }
    
    void preCreateRigidBody();
    
    void fracture();
    
    std::vector<Vertex> *frac_vertices;
	std::vector<Color> *frac_colors;
	std::vector<TexCoord> *frac_texCoords;
	std::vector<Normal> *frac_normals;
    
protected:
    // entity lifecycle
	bool onInitialize();
	void onPopulateVBOs();
	void onCreateRigidBody(btCollisionShape *sharedShape);
	void onPrepare(float dt);
	void onRender() const;
	void onPostRender();
	void onShutdown();
	void onCollision(CollidableEntity* collider);

private:
    bool m_fractured;
    Vector3 m_fractureDirection;
    float m_explodeSpeed;
    FractureObject *m_parent;
    int m_origCollisionFlags;
};


class FractureObject
{
public:
    FractureObject(CollidableEntity *entity, const string& fractureFile, float explodeSpeed);
    ~FractureObject();
    
    bool addToScene(Scene *scene);
    void prepare(float dt);
    void fracture() { m_shouldFracture = true; }
    
    map<long, Fracture*>* getFractures() { return m_fractureMap; }
    
    void simulationTick(float dt);

private:
    Scene *m_scene;
    CollidableEntity *m_origEntity;
    string m_fractureFile;
    map<long, Fracture*> *m_fractureMap;
    typedef map<long, Fracture*>::iterator FractureIterator;
    
	btPairCachingGhostObject *m_collisionGhost;
    bool m_shouldFracture;
    bool m_isFractured;
    float m_explodeSpeed;
};

#endif
