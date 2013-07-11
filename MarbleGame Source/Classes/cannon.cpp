#include "cannon.h"
#include "sphere.h"
#include "scene_graph.h"

Cannon::Cannon(Scene *scene, Vector3 barrelHalfExtents, float shootSpeed, float autoFire, float rotateSpeed) :
Cylinder(scene, barrelHalfExtents),
	m_shootSpeed(shootSpeed), m_autoFire(autoFire), m_refreshTime(autoFire), m_rotateSpeed(rotateSpeed)
{
	if (barrelHalfExtents.x != barrelHalfExtents.y) {
		GLOG("ERROR: cannon cannot have differe X,Y haldExtents\n");
		throw;
	}
}

void Cannon::onPrepare(float dt)
{
    Cylinder::onPrepare(dt); //currently empty
    
	// autofire behavior
    if (m_autoFire > 0.0f)
    {
        m_refreshTime -= dt;
        if (m_refreshTime <= 0.0f)
        {
            shootCannon();
            m_refreshTime = m_autoFire;
        }
    }

	////rotate behavior
	//if ( m_rotateSpeed > 0.0f){
	//	//update yaw
	//	setYaw(getYaw() + (m_rotateSpeed * dt));

	//	//get bullet representation
	//	btMotionState *ms = getRigidBody()->getMotionState();
	//	btTransform transform;
	//	ms->getWorldTransform(transform);
	//	//btTransform transform = btTransform(btQuaternion(Vector3(0.0f, 1.0f, 0.0f).btVec(), getYaw()));

	//	//update bullet representation
	//	btQuaternion q = btQuaternion(Vector3(0.0f, 1.0f, 0.0f).btVec(), getYaw());
	//	//btTransform newtrans;
	//	//getRigidBody()->getMotionState()->getWorldTransform(newtrans);
	//	//getRigidBody()->setWorldTransform(newtrans * transform);
	//	ms->setWorldTransform(transform * btTransform(q));
	//}

}

void Cannon::shootCannon()
{
	// turn off collision response to cylinder doesn't impede the cannonball
	getRigidBody()->setCollisionFlags(getRigidBody()->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);

	// create cannon ball
	Sphere *cannon_ball = new Sphere(m_scene, m_halfExtents.x - 0.1f);
	if (!cannon_ball->initialize(new LightTexShader(0.7f, 0.6f, 4, RAND_COLOR))) {
		GLOG("ERROR: could not create cannon ball");
		return;
	}
    cannon_ball->loadTexture("Textures/glass.tga");

	// position the cannon ball at the end of the barrel
	Vector3 look = s_Cam()->calculateLookDir(m_yaw + 90.0f, m_pitch);
	cannon_ball->setPosition(m_position);
	
	// add the cannon ball to the dynamic world and scene graph
	cannon_ball->addToDynamicWorld(m_dynamicWorld);
	m_scene->getGraph()->attach(cannon_ball);
	
	// shoot the cannon ball
	cannon_ball->getRigidBody()->setLinearVelocity((look * m_shootSpeed).btVec());
}