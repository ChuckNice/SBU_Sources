#include "entity.h"
#include "btBulletDynamicsCommon.h"
#include "shaders.h"

// static variable declaration
std::map<string, GLuint> Entity::s_textureMap;

Entity::Entity(Scene* const scene):
m_scene(scene),
	m_canBeRemoved(false),
	m_position(Vector3(0.0f, 0.0f, 0.0f)),
	m_yaw(0.0f),
	m_pitch(0.0f),
	m_roll(0.0f),
	m_rigidBody(NULL),
	m_texture0(0),
	m_graphNode(NULL),
    m_isInitialized(false),
m_vertexVBO(0),
m_colorVBO(0),
m_texCoordVBO(0),
m_normalVBO(0)
{
}

Entity::~Entity()
{
}

bool Entity::initialize(Shader *shader)
{
	ShaderProgram = shader;

	if ( !onInitialize() )
		return false;

	// have subclass populate VBO vectors
	// If the entity is an imported OBJmodel we'll skip most of this because
	// the model handles the steps itself
	onPopulateVBOs();
	if (m_vertices.size() <= 0) {
		GLOG("ERROR: Entity subclass did not provide vertex data in onPopulateVBOs method\n");
		return false;
	}

	// send vertices to GPU
	glGenBuffers(1, &m_vertexVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO); //Bind the vertex buffer
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * m_vertices.size(), &m_vertices[0], GL_STATIC_DRAW); //Send the data to OpenGL

	// send colors to GPU if we have them
	if ( ShaderProgram->supportsColoring() ) {
		if (!hasColors()) { GLOG("Shader requires colors but none were supplied\n"); throw; }
		glGenBuffers(1, &m_colorVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_colorVBO); //Bind the color buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 4 * m_colors.size(), &m_colors[0], GL_STATIC_DRAW); //Send the data to OpenGL
	}

	// send texCoords to GPU if we have them
	if ( ShaderProgram->supportsTexuring() ) {
		if (!hasTexCoords()) { GLOG("Shader requires texCoords but none were supplied\n"); throw; }
		glGenBuffers(1, &m_texCoordVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVBO); //Bind the texcoord buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 2 * m_texCoords.size(), &m_texCoords[0], GL_STATIC_DRAW); //Send the data to OpenGL
	}

	// send normals to GPU if we have them
	if ( ShaderProgram->supportsLighting() ) {
		if (!hasNormals()) { GLOG("Shader requires normals but none were supplied\n"); throw; }
		glGenBuffers(1, &m_normalVBO);
		glBindBuffer(GL_ARRAY_BUFFER, m_normalVBO); //Bind the normal buffer
		glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 3 * m_normals.size(), &m_normals[0], GL_STATIC_DRAW); //Send the data to OpenGL
	}

    m_isInitialized = true;
	return true;
}

bool Entity::loadTexture(const string& textureFile)
{
	// only load the texture if it hasn't been loaded already
	if ( !s_textureMap[textureFile] )
	{
		TargaImage image;
		if( !image.load(textureFile) ) {
			return false;
		}
		glGenTextures(1, &m_texture0);
		glBindTexture(GL_TEXTURE_2D, m_texture0);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#ifndef MARBLE_IOS_BUILD
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
        gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, image.getWidth(), image.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, image.getImageData());
#else
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, image.getWidth(), image.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, image.getImageData());
#endif

		s_textureMap[textureFile] = m_texture0;
	}

    m_texture0File = textureFile;
	m_texture0 = s_textureMap[textureFile];

	return true;
}

void Entity::prepare(float dt)
{
    // update our position from bullet so other things can query it
    // NRTODO: this  should update yaw, pitch, roll also but then we can't manually set that for the player
    if ( m_rigidBody && m_rigidBody->getMotionState() )
	{
		btTransform worldTransform;
		m_rigidBody->getMotionState()->getWorldTransform(worldTransform);
        m_position = Vector3(worldTransform.getOrigin());
	}
    
    ShaderProgram->update(dt);
	onPrepare(dt);
}

void Entity::render() const
{
	ShaderProgram->bindShader();

	// pointer for modifying member variables
	Entity *self = const_cast<Entity*>(this);

	// binet the vertex pointer
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, self->m_vertexVBO);
	glVertexAttribPointer((GLint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	// bind the color pointer, if we have
	if (ShaderProgram->supportsColoring()) { 
		glEnableVertexAttribArray(1); 
		glBindBuffer(GL_ARRAY_BUFFER, self->m_colorVBO);
		glVertexAttribPointer((GLint)1, 4, GL_FLOAT, GL_FALSE, 0, 0);
	}

	// bind the texcoord pointer, if we have
	if (ShaderProgram->supportsTexuring()) { 
		glEnableVertexAttribArray(2); 
		glBindBuffer(GL_ARRAY_BUFFER, self->m_texCoordVBO);
		glVertexAttribPointer((GLint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);

		// set the texture if one is loaded
		glEnable(GL_TEXTURE_2D);
		if ( m_texture0 ) {
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, m_texture0);
		}
	}

	// bind the normal pointer, if we have
	if (ShaderProgram->supportsLighting()) { 
		glEnableVertexAttribArray(3); 
		glBindBuffer(GL_ARRAY_BUFFER, self->m_normalVBO);
		glVertexAttribPointer((GLint)3, 3, GL_FLOAT, GL_FALSE, 0, 0);

		// apply lighting to the shader
		s_Lighting()->applyLighting(ShaderProgram);
	}

	// enable blending if needed
	if (ShaderProgram->usesBlending()) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	// determine modelview (either from bullet or the position/pitch/yaw) 
	// Based in part on whether the entity is constructed as dynamic ( as in OBJmodel 'isDynamic' )
	// TODOCV:This function still needs more fine tuning. (i.e. If we have a rotating object that should also be dynamic AND controlled by Bullet Physics)
	Matrix4 modelview = *s_Cam()->modelviewMatrix();

	if (m_rigidBody && 
		m_rigidBody->getMotionState() //&& 
		//((m_rigidBody->getCollisionFlags() & btCollisionObject::CF_STATIC_OBJECT) != btCollisionObject::CF_STATIC_OBJECT) //|| //this allows cannons and collectibles to spin for final presentation, BUT it breaks the button animation	
		//((m_rigidBody->getCollisionFlags() & btCollisionObject::CO_GHOST_OBJECT) == btCollisionObject::CO_GHOST_OBJECT) 
		)
	{
		// get world transform from bullet and apply it to camera modelview
		btTransform worldTransform;
		m_rigidBody->getMotionState()->getWorldTransform(worldTransform);
		modelview *= Matrix4(worldTransform);
	}
	else
	{
		// transform the entity to it's position, yaw, and pitch
		modelview *= self->getModelviewMatrix();
	}

	// send matrices to shader program
	ShaderProgram->sendUniform4x4("modelview_matrix", modelview.mat);
	ShaderProgram->sendUniform4x4("projection_matrix", s_Cam()->projectionMatrix()->mat);
	if (ShaderProgram->supportsLighting()) { ShaderProgram->sendUniform3x3("normal_matrix", &(modelview.calculateNormalMatrix()[0])); }

	onRender();

	glDrawArrays(self->getRenderingMode(), 0, (int)m_vertices.size());

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
	// cleanup
	glDisableVertexAttribArray(0);
	if (ShaderProgram->supportsColoring()) { glDisableVertexAttribArray(1); }
	if (ShaderProgram->supportsTexuring()) { 
		glDisableVertexAttribArray(2);
		glBindTexture(GL_TEXTURE_2D, 0);
		glDisable(GL_TEXTURE_2D);
	}
	if (ShaderProgram->supportsLighting()) { glDisableVertexAttribArray(3); }
	if (ShaderProgram->usesBlending()) { glDisable(GL_BLEND); }
	glUseProgram(0);
}

void Entity::postRender()
{
	onPostRender();
}

void Entity::simulationTick(float dt)
{
	onSimulationTick(dt);
}

void Entity::shutdown()
{
    if (ShaderProgram != NULL && !usesSharedShader())
        delete ShaderProgram;
    
	// delete our buffers
	glDeleteBuffers(1, &m_vertexVBO);
	if (m_colorVBO) { glDeleteBuffers(1, &m_colorVBO); }
	if (m_texCoordVBO) { glDeleteBuffers(1, &m_texCoordVBO); }
	if (m_normalVBO) { glDeleteBuffers(1, &m_normalVBO); }
    
    // NRTODO: super small memory leak because we can't delete texture

	onShutdown();
}


Matrix4 Entity::getModelviewMatrix()
{
	Matrix4 modelview;
	modelview.translatef(m_position.x, m_position.y, m_position.z);
	modelview.rotatef(m_yaw, 0.0f, -1.0f, 0.0f);
	modelview.rotatef(m_pitch, -1.0f, 0.0f, 0.0f);
	modelview.rotatef(m_roll, 0.0f, 0.0f, -1.0f);
	return modelview;
}