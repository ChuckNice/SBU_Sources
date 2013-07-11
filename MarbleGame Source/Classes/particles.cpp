/*
 *  ParticleSystem.cpp
 *  ParticleSystem
 *
 *  Created by Nick Rudolfsky on 2/9/11.
 */

#include "particles.h"
#include "scene_graph.h"

// static variable declaration
std::map<string, GLuint> ParticleSystem::s_textureMap;


ParticleSystem::ParticleSystem(int numParticles) :
particleIndex(0),
m_sphericalSystem(false)
{
	active = false;
	totalParticles = numParticles;
	particleCount = 0;
	particleTimer = 0;
	forceWhite = false;
	movesInWorld = false;
    
    ShaderProgram = new GlowShader(Color(1.0f, 1.0f, 1.0f, 1.0f));
    
    //allocation memory for particles array
	particles = (Particle *)malloc(sizeof(Particle) * totalParticles);
    
    // send vertices
    Vector3 vertices[4] = {
        Vector3(-1.0f, -1.0f, 0.0f),
        Vector3(-1.0f,  1.0f, 0.0f),
        Vector3( 1.0f, -1.0f, 0.0f),
        Vector3( 1.0f,  1.0f, 0.0f)
    };
    glGenBuffers(1, &m_vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * 4, &vertices[0], GL_STATIC_DRAW);
    
    // send tex coords
    TexCoord texCoords[4] = {
        TexCoord(0.0f, 0.0f),
        TexCoord(0.0f, 1.0f),
        TexCoord(1.0f, 0.0f),
        TexCoord(1.0f, 1.0f)
    };
    glGenBuffers(1, &m_texCoordVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * 4, &texCoords[0], GL_STATIC_DRAW);
}


ParticleSystem::~ParticleSystem()
{
	free(particles);
}


void ParticleSystem::initializeParticle(Particle *particle)
{
	//set initial lifetime
	particle->lifetime = lifetime + lifetimeVar * RAND_NEG1_1;
	if (particle->lifetime < 0) { particle->lifetime = 0; }
	
    if (!m_sphericalSystem) {
        //set initial position
        particle->position = Vector3(emitterPosition.x + emitterPositionVar.x * RAND_NEG1_1,
                                     emitterPosition.y + emitterPositionVar.y * RAND_NEG1_1,
                                     emitterPosition.z + emitterPositionVar.z * RAND_NEG1_1);
        
        //set velocity based on direction and speed
        float particleSpeed = speed + speedVar * RAND_NEG1_1;
        Vector3 direction = s_Cam()->calculateLookDir(emitterOrientation.x + emitterOrientationVar.x * RAND_NEG1_1,
                                                      emitterOrientation.y + emitterOrientationVar.y * RAND_NEG1_1);
        particle->velocity = direction * particleSpeed;
    }
    else {
        // initial position in spherical system is based on emmiterOreintation
        Vector3 direction = s_Cam()->calculateLookDir(emitterOrientation.x + emitterOrientationVar.x * RAND_NEG1_1,
                                                      emitterOrientation.y + emitterOrientationVar.y * RAND_NEG1_1);
        particle->position = direction * (ROOT_NODE_BOUNDS - 50.0f);
        
        // no velocity in speherical system
        particle->velocity = Vector3(0.0f, 0.0f, 0.0f);
    }
	
	//set the color and color delta
	particle->color = Color(startColor.r + startColorVar.r * RAND_NEG1_1,
                            startColor.g + startColorVar.g * RAND_NEG1_1,
                            startColor.b + startColorVar.b * RAND_NEG1_1,
                            startColor.a + startColorVar.a * RAND_NEG1_1);
	Color endColor2 = Color(endColor.r + endColorVar.r * RAND_NEG1_1,
                               endColor.g + endColorVar.g * RAND_NEG1_1,
                               endColor.b + endColorVar.b * RAND_NEG1_1,
                               endColor.a + endColorVar.a * RAND_NEG1_1);
	particle->colorDelta = Color(((endColor2.r - particle->color.r) / particle->lifetime),
                                 ((endColor2.g - particle->color.g) / particle->lifetime),
                                 ((endColor2.b - particle->color.b) / particle->lifetime),
                                 ((endColor2.a - particle->color.a) / particle->lifetime));
	
	//set size and size delta
	particle->size = startSize + startSizeVar * RAND_NEG1_1;
	if (particle->size < 0) { particle->size = 0; }
	float particleEndSize = endSize + endSizeVar * RAND_NEG1_1;
	particle->sizeDelta = (particleEndSize - particle->size) / particle->lifetime;
}



void ParticleSystem::update(float dt)
{
	if (active && emissionRate != 0) {
		particleTimer += dt;
		float timePerParticle = 1.0f / emissionRate;
		while( particleCount < totalParticles && particleTimer > timePerParticle )
        {
			//get address to next particle in array and initialize
			Particle *particle = &particles[particleCount];
			initializeParticle(particle);
			particleCount++;
			
			particleTimer -= timePerParticle;
		}
	}

    // iterate through each particle
	particleIndex = 0;
	while (particleIndex < particleCount)
    {
		//get reference to particle at index
		Particle *particle = &particles[particleIndex];
		
		if (particle->lifetime <= 0) {
			//particle is dead, put at end of array to be reinitialized
			if( particleIndex != particleCount-1 )
				particles[particleIndex] = particles[particleCount-1];
			particleCount--;
		}
		else {
			//particle is alive, update state
			particle->position += particle->velocity * dt;
						
			particle->color.r += particle->colorDelta.r * dt;
			particle->color.g += particle->colorDelta.g * dt;
			particle->color.b += particle->colorDelta.b * dt;
			particle->color.a += particle->colorDelta.a * dt;
			
			particle->size += particle->sizeDelta * dt;
			if (particle->size < 0)
				particle->size = 0;
			
			particle->lifetime -= dt;
			
			particleIndex++;
		}
		
		//test if any particles are alive
		if (particleCount == 0) {
			//all particles are dead
		}
	}
}



void ParticleSystem::render()
{
	ShaderProgram->bindShader();
    if (getProjectionType() == PT_PERSPECTIVE)
        ShaderProgram->sendUniform4x4("projection_matrix", s_Cam()->projectionMatrix()->mat);
    else
        ShaderProgram->sendUniform4x4("projection_matrix", s_Cam()->orthoProjectionMatrix()->mat);
    
    // send vertices to shader
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
    glVertexAttribPointer((GLint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
	// send texcoords to shader
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVBO);
    glVertexAttribPointer((GLint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    if ( texID ) {
        glEnable(GL_TEXTURE_2D);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texID);
    }
    
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    if (m_additive)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    else
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Good for Smoke
    
    // render each particle
    for (int i = 0; i < particleIndex - 1; i++)
    {
        if (particles[i].lifetime > 0)
        {
            // send particles modelview matrix to shader
            Matrix4 modelview = *s_Cam()->modelviewMatrix();
            modelview.translatef(particles[i].position.x, particles[i].position.y, particles[i].position.z);
            modelview.rotatef(s_Cam()->getYaw() - 90.0f, 0.0f, -1.0f, 0.0f);
            modelview.rotatef(s_Cam()->getPitch(), -1.0f, 0.0f, 0.0f);
            modelview.scalef(particles[i].size);
            ShaderProgram->sendUniform4x4("modelview_matrix", modelview.mat);
            
            // send particles color to shader
            ShaderProgram->sendUniform("color", particles[i].color.r, particles[i].color.g, particles[i].color.b, particles[i].color.a);
            
            glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
        }
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    // cleanup
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    if ( texID ) {
        glDisable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);
	glUseProgram(0);
}




// method that loads a particle texture ONLY if not loaded already
GLuint ParticleSystem::loadParticleTexture(const string& textureFile)
{
    GLuint texID = 0;
    
    // only load the texture if it hasn't been loaded already
	if ( !s_textureMap[textureFile] )
	{
		TargaImage image;
		if( !image.load(textureFile) ) {
			return 0;
		}
		glGenTextures(1, &texID);
		glBindTexture(GL_TEXTURE_2D, texID);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#ifndef MARBLE_IOS_BUILD
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, image.getWidth(), image.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, image.getImageData());
#else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), image.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.getImageData());
#endif
        
		s_textureMap[textureFile] = texID;
	}
    
	texID = s_textureMap[textureFile];
    
	return texID;
}


