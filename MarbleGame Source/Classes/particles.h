/*
 *  ParticleSystem.h
 *  ParticleSystem
 *
 *  Created by Nick Rudolfsky on 2/9/11.
 */

#ifndef __E4ParticleSystem__
#define __E4ParticleSystem__

#include "geom.h"
#include "global.h"

// this struct represents a single particle
typedef struct _Particle {
	// the lifetime of a particle
	float lifetime;
	// position and position change rate (velocity)
	Vector3 position;
	Vector3 velocity;
	// color and color change rate
	Color color;
	Color colorDelta;
	// size and size change rate
	float size;
	float sizeDelta;
} Particle;


// enumeration of rendering projections
typedef enum _projection_type
{
    PT_PERSPECTIVE = 0,
    PT_ORTHOGRAPHIC
} ProjectionType;


// this class represent a particle system comprised of E4Particles.
class ParticleSystem {
public:
	// constructor.
	ParticleSystem(int numParticles);
	// destrcutor
	~ParticleSystem();
	
public:
	// update method
	void update(float dt);
	// render method
	void render();
    
    // call this method to start the particle system
	void startSystem() { active = true; }
	// call this method to stop the particle system
	void stopSystem() { active = false; }
    // method to move particleSystem to a new position
	void moveToPoint(float x, float y, float z) { emitterPosition = Vector3(x, y, z); }
    // set the texture for this system
    void setParticleTexture(const string& textureFile) { texID = loadParticleTexture(textureFile); }
	// if oyu need to know if the particle system is active call this
	bool isActive(void) { return active; }
	
public: // set these parameters in subclasses init method
    // texture for particles (should set with setParticleTexture method)
	int texID;
	// the position of the emitter
	Vector3 emitterPosition;
	Vector3 emitterPositionVar;
	// orientation the emmiter emits particles in degrees (yaw, pitch, roll)
	Vector3 emitterOrientation;
	Vector3 emitterOrientationVar;
	// the speed the particles will move at
	float speed;
	float speedVar;
	// the starting color of the particles
	Color startColor;
	Color startColorVar;
	// the ending color od the particles
	Color endColor;
	Color endColorVar;
	// the starting size of the particles
	float startSize;
	float startSizeVar;
	// the end size of the particles
	float endSize;
	float endSizeVar;
	// the lifetime of the particles
	float lifetime;
	float lifetimeVar;
	// the emission rate of the particles (emmisions per second)
	float emissionRate;
    // determines if particles are additive-blended or regular blending
    bool m_additive;
    // determines if particle position is determined by emitterPosition or emmiterOreintation (orientation used to make surrounding particle systems like starry sky)
    bool m_sphericalSystem;
	// bool indicating if we should force white
	bool forceWhite;
	// bool indicating if system moves in the world
	bool movesInWorld;
    
    virtual ProjectionType getProjectionType() { return PT_PERSPECTIVE; }
	
protected:
	// the array of particles
	Particle *particles;
    // the shader used to render particles
    Shader *ShaderProgram;
	// total number of particles system can have
	int totalParticles;
	// number of particles currently in system
	int particleCount;
	// variable to track how many particles we must emit in update method
	float particleTimer;
	// variable to track index of particle in update method
	int particleIndex;
    // a boolean indicating if system is active
	bool active;
	
private:
	// method to initialize a particle
	void initializeParticle(Particle *particle);
    
    // static texture map used to prevent images from being loaded multiple times across all entities
	static std::map<string, GLuint> s_textureMap;
    // method that loads a particle texture ONLY if not loaded already
    GLuint loadParticleTexture(const string& textureFile);
    
    // vertex data
    GLuint m_vertexVBO;
	GLuint m_texCoordVBO;
};

#endif