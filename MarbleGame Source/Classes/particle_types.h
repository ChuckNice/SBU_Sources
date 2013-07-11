/*
 *  E4FireParticleSystem.h
 *  E4ParticleSystem
 *
 *  Created by Nitrex88 on 2/10/11.
 *  Copyright 2011 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __E4FireParticleSystem__
#define __E4FireParticleSystem__

#include "particles.h"


class FireParticleSystem : public ParticleSystem {
public:
	FireParticleSystem(int numParticles)
    : ParticleSystem(numParticles)
    {
        // emitter position
        emitterPosition = Vector3(0.0f, 0.0f, 0.0f);
        emitterPositionVar = Vector3(1.0f, 0.25f, 1.0f);
        
        // orientation (yaw, pitch, roll)
        emitterOrientation = Vector3(0.0f, 90.0f, 0.0f);
        emitterOrientationVar = Vector3(0.0f, 20.0f, 0.0f);
        
        // life of particles
        lifetime = 2;
        lifetimeVar = 0.0;
        
        // speed of particles
        speed = 2.0;
        speedVar = 1.0;
        
        // size, in pixels
        startSize = 0.5f;
        startSizeVar = 0.2f;
        endSize = 0.4f;
        endSizeVar = 0.1f;
        
        // emits per frame
        emissionRate = totalParticles/lifetime;
        
        // color of particles
        startColor.r = 0.96f;
        startColor.g = 0.25f;
        startColor.b = 0.12f;
        startColor.a = 1.0f;
        startColorVar.r = 0.0f;
        startColorVar.g = 0.0f;
        startColorVar.b = 0.0f;
        startColorVar.a = 0.0f;
        endColor.r = 0.0f;
        endColor.g = 0.0f;
        endColor.b = 0.0f;
        endColor.a = 1.0f;
        endColorVar.r = 0.0f;
        endColorVar.g = 0.0f;
        endColorVar.b = 0.0f;
        endColorVar.a = 0.0f;
        
        m_additive = true;
        
        setParticleTexture("Textures/fire.tga");
    }
};


class RandomParticleSystem : public ParticleSystem {
public:
	RandomParticleSystem(int numParticles)
    : ParticleSystem(numParticles)
    {
        // emitter position
        emitterPosition = Vector3(0.0f, 0.0f, 0.0f);
        emitterPositionVar = Vector3(2.0f, 0.25f, 2.0f);
        
        // orientation (yaw, pitch, roll)
        emitterOrientation = Vector3(0.0f, 0.0f, 0.0f);
        emitterOrientationVar = Vector3(360.0f, 360.0f, 0.0f);
        
        // life of particles
        lifetime = 3;
        lifetimeVar = 0;
        
        // speed of particles
        speed = 1.0;
        speedVar = 0.5;
        
        // size, in pixels
        startSize = 0.5f;
        startSizeVar = 0.2f;
        endSize = 0.2f;
        endSizeVar = 0.2f;
        
        // emits per frame
        emissionRate = totalParticles/lifetime;
        
        // color of particles
        startColor.r = 0.5f;
        startColor.g = 0.5f;
        startColor.b = 0.5f;
        startColor.a = 1.0f;
        startColorVar.r = 0.5f;
        startColorVar.g = 0.5f;
        startColorVar.b = 0.5f;
        startColorVar.a = 0.0f;
        endColor.r = 0.5f;
        endColor.g = 0.5f;
        endColor.b = 0.5f;
        endColor.a = 1.0f;
        endColorVar.r = 0.5f;
        endColorVar.g = 0.5f;
        endColorVar.b = 0.5f;
        endColorVar.a = 0.0f;
        
        m_additive = true;
        
        setParticleTexture("Textures/fire.tga");
    }
};


class PlayerParticleSystem : public ParticleSystem {
public:
	PlayerParticleSystem(int numParticles)
    : ParticleSystem(numParticles)
    {
        // emitter position
        emitterPosition = Vector3(0.0f, 0.0f, 0.0f);
        emitterPositionVar = Vector3(0.1f, 0.1f, 0.0f);
        
        // orientation (yaw, pitch, roll)
        emitterOrientation = Vector3(0.0f, 90.0f, 0.0f);
        emitterOrientationVar = Vector3(360.0f, 70.0f, 0.0f);
        
        // life of particles
        lifetime = 1;
        lifetimeVar = 1;
        
        // speed of particles
        speed = 1.5;
        speedVar = 0.5;
        
        // size, in pixels
        startSize = 0.2f;
        startSizeVar = 0.2f;
        endSize = 0.1f;
        endSizeVar = 0.1f;
        
        // emits per frame
        emissionRate = totalParticles/lifetime;
        
        // color of particles
        startColor.r = 0.5f;
        startColor.g = 0.5f;
        startColor.b = 0.5f;
        startColor.a = 1.0f;
        startColorVar.r = 0.5f;
        startColorVar.g = 0.5f;
        startColorVar.b = 0.5f;
        startColorVar.a = 0.0f;
        endColor.r = 0.5f;
        endColor.g = 0.5f;
        endColor.b = 0.5f;
        endColor.a = 1.0f;
        endColorVar.r = 0.5f;
        endColorVar.g = 0.5f;
        endColorVar.b = 0.5f;
        endColorVar.a = 0.0f;
        
        m_additive = true;
        
        setParticleTexture("Textures/fire.tga");
    }
};



class StarryParticleSystem : public ParticleSystem {
public:
	StarryParticleSystem(int numParticles)
    : ParticleSystem(numParticles)
    {
        // emitter position
        emitterPosition = Vector3(0.0f, 0.0f, 0.0f);
        emitterPositionVar = Vector3(0.0f, 0.0f, 0.0f);
        
        // orientation (yaw, pitch, roll)
        emitterOrientation = Vector3(0.0f, 0.0f, 0.0f);
        emitterOrientationVar = Vector3(360.0f, 360.0f, 0.0f);
        
        // life of particles
        lifetime = 6;
        lifetimeVar = 5;
        
        // speed of particles
        speed = 0.0;
        speedVar = 0.0;
        
        // size, in pixels
        startSize = 5.0f;
        startSizeVar = 0.1f;
        endSize = 5.0f;
        endSizeVar = 0.1f;
        
        // emits per frame
        emissionRate = totalParticles/lifetime;
        
        // color of particles
        startColor.r = 0.0f;
        startColor.g = 0.0f;
        startColor.b = 0.0f;
        startColor.a = 1.0f;
        startColorVar.r = 0.0f;
        startColorVar.g = 0.0f;
        startColorVar.b = 0.0f;
        startColorVar.a = 0.0f;
        endColor.r = 1.0f;
        endColor.g = 1.0f;
        endColor.b = 1.0f;
        endColor.a = 0.0f;
        endColorVar.r = 0.0f;
        endColorVar.g = 0.0f;
        endColorVar.b = 0.0f;
        endColorVar.a = 0.0f;
        
        m_additive = true;
        m_sphericalSystem = true;
        
        setParticleTexture("Textures/fire.tga");
    }
};



class CharlieParticleSystem : public ParticleSystem {
public:
	CharlieParticleSystem(int numParticles)
		: ParticleSystem(numParticles)
	{
		// emitter position
		emitterPosition = Vector3(0.0f, 0.0f, 0.0f);
		emitterPositionVar = Vector3(0.0f, 0.0f, 0.0f);

		// orientation (yaw, pitch, roll)
		emitterOrientation = Vector3(0.0f, 90.0f, 0.0f);
		emitterOrientationVar = Vector3(360.0f, 30.0f, 0.0f);

		// life of particles
		lifetime = 2;
		lifetimeVar = 0.0;

		// speed of particles
		speed = 4.0;
		speedVar = 1.0;

		// size, in pixels
		startSize = 0.1f;
		startSizeVar = 0.0f;
		endSize = 0.7f;
		endSizeVar = 0.1f;

		// emits per frame
		emissionRate = totalParticles/lifetime;

		// color of particles
		startColor.r = 0.15f;
		startColor.g = 0.25f;
		startColor.b = 0.12f;
		startColor.a = 1.0f;
		startColorVar.r = 0.2f;
		startColorVar.g = 0.2f;
		startColorVar.b = 0.7f;
		startColorVar.a = 0.0f;
		endColor.r = 0.0f;
		endColor.g = 0.0f;
		endColor.b = 0.0f;
		endColor.a = 1.0f;
		endColorVar.r = 0.0f;
		endColorVar.g = 0.0f;
		endColorVar.b = 0.0f;
		endColorVar.a = 0.0f;

		setParticleTexture("Textures/fire.tga");
	}
};


class WaypointParticleSystem : public ParticleSystem {
public:
	WaypointParticleSystem(int numParticles)
    : ParticleSystem(numParticles)
    {
        // emitter position
        emitterPosition = Vector3(0.0f, 0.0f, 0.0f);
        emitterPositionVar = Vector3(1.0f, 0.25f, 1.0f);
        
        // orientation (yaw, pitch, roll)
        emitterOrientation = Vector3(0.0f, 90.0f, 0.0f);
        emitterOrientationVar = Vector3(0.0f, 20.0f, 0.0f);
        
        // life of particles
        lifetime = 0.5;
        lifetimeVar = 0.0;
        
        // speed of particles
        speed = 20.0;
        speedVar = 1.0;
        
        // size, in pixels
        startSize = 0.5f;
        startSizeVar = 0.2f;
        endSize = 0.4f;
        endSizeVar = 0.1f;
        
        // emits per frame
        emissionRate = totalParticles/lifetime;
        
        // color of particles
        startColor.r = 0.96f;
        startColor.g = 0.25f;
        startColor.b = 0.12f;
        startColor.a = 1.0f;
        startColorVar.r = 0.0f;
        startColorVar.g = 0.0f;
        startColorVar.b = 0.0f;
        startColorVar.a = 0.0f;
        endColor.r = 0.0f;
        endColor.g = 0.0f;
        endColor.b = 0.0f;
        endColor.a = 1.0f;
        endColorVar.r = 0.0f;
        endColorVar.g = 0.0f;
        endColorVar.b = 0.0f;
        endColorVar.a = 0.0f;
        
        m_additive = true;
        
        setParticleTexture("Textures/fire.tga");
    }
};



#endif
