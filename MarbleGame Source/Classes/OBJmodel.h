#ifndef __OBJ_MODEL_H__
#define __OBJ_MODEL_H__

#include "collidable_entity.h"

class Scene;

class OBJmodel :public CollidableEntity
{
public: 
    OBJmodel(Scene *scene, string objFile, float scale = 1.0f, bool useAABBApproximation = false, bool isDynamic = false, bool isFaceCulled = true);
    EntityType getType() { return ET_OBJ_MODEL; }
    CullingType getCullingType() { return CT_AABB; }

    // entity lifecycle
    virtual bool onInitialize();
    virtual void onPopulateVBOs();
    virtual void onCreateRigidBody( btCollisionShape *sharedShape );
    virtual void onPrepare( float dt );
    void onRender() const;
    void onPostRender();
    void onShutdown();
    virtual void onCollision( CollidableEntity* collider );
    
    Vector3 getSize() { return m_size; }

protected:
    Vector3 m_size;
    float m_scale;

private:
    string m_objFile;
    string m_materialFile;

	bool m_isDynamic;
	bool m_useAABBApproximation;
	bool m_isFaceCulled;
};

#endif //__OBJ_MODEL_H__