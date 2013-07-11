#ifndef __WAYPOINT_TYPES_H__
#define __WAYPOINT_TYPES_H__

#include "waypoint.h"
#include "player.h"
#include "particle_types.h"

//// Different type of waypoints and their behavior specs

// CHECKPOINT 
// updates the currently reached checkpoint of the player upon collision
class Checkpoint : 
	public Waypoint
{
public:
	// Constructor
	Checkpoint(Scene *scene, string filename, float scale) : 
	  Waypoint(scene, filename, scale, false, false, true) {
		m_particleCooldown = 75;
		m_secondaryCooldown = 150; //not used
	  }
	
	// Overridden funcs
	WP_Type getWaypointType() { return WP_CHECKPOINT; }

	void onCollision(CollidableEntity* collider){

		// if the player collides, then update the players last checkpoint reached
		// and change color of the partical system from grey-scale to a random color
		if ( collider != NULL && collider->getType() == ET_PLAYER){
			
			Player* self = dynamic_cast<Player*>(collider); 
			if( self->getLastCheckpoint() != this ){
				self->setCheckpoint(this);
			}

			// morph partical color from grey to random color
			// TODOCV: not yet implemented
			if( m_particleSys){
				m_particleSys->startSystem();
			}
		}		
	}

	void onPrepare( float dt )
	{
		if ( m_particleSys->isActive() ){
			if (--m_particleCooldown < 0)
			{

				// life of particles
				m_particleSys->lifetime = 1;
				m_particleSys->lifetimeVar = 0.5;

				// speed of particles
				m_particleSys->speed = 10.0;
				m_particleSys->speedVar = 5.0f;

				// size, in pixels
				/*startSize = 0.1f;
				startSizeVar = 0.0f;
				endSize = 0.7f;
				endSizeVar = 0.1f;*/

				// emits per frame
				m_particleSys->emissionRate = 15.0f;

				if (--m_secondaryCooldown < 0){
					//m_particleSys->stopSystem();
				}
			}
		}
	}

private:
	int m_particleCooldown;
	int m_secondaryCooldown;
};

// START
// add this to the level to begin the player here
class Start :
	public Waypoint
{
public: 
	// Constructor
	Start(Scene *scene, string filename, float scale) : 
	  Waypoint(scene, filename, scale, false, false, true) 
	  {
		  m_particleCooldown = 10;
	  }

	  // Overridden funcs
	  WP_Type getWaypointType() { return WP_START; }

	  void onCollision(CollidableEntity* collider)
	  {
		  // if the player collides, then update the players last checkpoint reached
		  // and run short lived partical system
		  if ( collider != NULL && collider->getType() == ET_PLAYER){
			  Player* self = dynamic_cast<Player*>(collider); 
			  if( self->getLastCheckpoint() != this ){
					self->setCheckpoint(this);
			  }
			  // run the partical system
			  // TODOCV: not yet implemented
			  if( m_particleSys)
			  {
				  m_particleSys->startSystem();
			  }
		  }
	  }

	  void onPrepare( float dt )
	  {
		  if ( m_particleSys)
		  {
			  if (--m_particleCooldown < 0){
				  m_particleSys->stopSystem();
			  }
		  }
	  }

private:
	int m_particleCooldown;
};



class NickCheckPoint : public Waypoint
{
public:
	// Constructor
	NickCheckPoint(Scene *scene, string filename, float scale) :
    Waypoint(scene, filename, scale, false, false, true) { }
	WP_Type getWaypointType() { return WP_CHECKPOINT; }
    
    void attachParticleSystem( ParticleSystem *system)
    {
        m_particleSys = system;
        m_particleSys->startColorVar = Color(0.01f, 0.01f, 0.01f, 0.2f);
        m_particleSys->endColorVar = Color(0.01f, 0.01f, 0.01f, 0.2f);
        m_particleSys->speed = 0.5f;
        m_particleSys->startSystem();
        m_particleSys->moveToPoint(getPosition().x, getPosition().y, getPosition().z);
        m_scene->getGraph()->attach(m_particleSys);
    }
    
	void onCollision(CollidableEntity* collider)
    {
		if ( collider != NULL && collider->getType() == ET_PLAYER)
		{
            // set player checkpoint
			Player* self = dynamic_cast<Player*>(collider);
			if( self->getLastCheckpoint() != this ) {
				self->setCheckpoint(this);
			}
            
			// morph partical color from grey to random color
			if( m_particleSys) {
				m_particleSys->startColorVar = Color(0.5f, 0.5f, 0.5f, 0.0f);
                m_particleSys->endColorVar = Color(0.5f, 0.5f, 0.5f, 0.0f);
                m_particleSys->speed = 3.0f;
			}
		}
	}
};


class FinishCheckPoint : public Waypoint
{
public:
	// Constructor
	FinishCheckPoint(Scene *scene, string filename, float scale) :
    Waypoint(scene, filename, scale, false, false, true) { }
	WP_Type getWaypointType() { return WP_FINISH; }
    
	void onCollision(CollidableEntity* collider)
    {
		if ( collider != NULL && collider->getType() == ET_PLAYER)
		{
            // set player checkpoint
			Player* self = dynamic_cast<Player*>(collider);
			if( self->getLastCheckpoint() != this ) {
				self->setCheckpoint(this);
			}
            
            s_Cam()->setCameraMode(CM_ORBIT);
            m_scene->finishedLevel();
		}
	}
};


#endif  

