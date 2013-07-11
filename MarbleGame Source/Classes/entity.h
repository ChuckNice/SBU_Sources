#ifndef ENTITY_H_INCLUDED
#define ENTITY_H_INCLUDED

#include "geom.h"
#include "global.h"
#include "entity_types.h"
#include <map>

class Scene;
class btRigidBody;

/**
This is the base class for any object in a scene. It manages properties
that are common to all objects (such as position, yaw, and pitch) and provides
a set of virtual methods that are called on every frame. Entities of any type can be
attached to the scene graph which will handle culling and pass entities to the renderer.
*/
class Entity 
{
public:
	Entity(Scene* const scene);
	virtual ~Entity();

	// entity lifecycle
	virtual bool	initialize(Shader *shader);
	virtual void	prepare(float dt);
	void			render() const;
	void			postRender();
	virtual void	simulationTick(float dt);
	void			shutdown();
	virtual EntityType	getType() = 0;
	virtual CullingType getCullingType() = 0;

    // the rendering mode should only be overridden if an entity subclass lays out vertices in a different mode (ie. GL_TRIANGLE_STRIP)
	virtual GLenum getRenderingMode() { return GL_TRIANGLES; }
    virtual bool usesSharedShader() { return false; }

	// this function will load a texture, if it hasn't been loaded already for another entity
	bool loadTexture(const string& textureFile);

	// position, yaw, and pitch
	Vector3 getPosition() const { return m_position; }
	void	setPosition(const Vector3& position) { m_position = position; }
	float	getYaw() const { return m_yaw; }
	void	setYaw(const float& yaw) { m_yaw = yaw; }
	float	getPitch() const { return m_pitch; }
	void	setPitch(const float& pitch) { m_pitch = pitch; }
	float	getRoll() const { return m_roll; }
	void	setRoll(const float& roll) { m_roll = roll; }
	void	setOrientation(const float& yaw, const float& pitch, const float& roll) { setYaw(yaw); setPitch(pitch); setRoll(roll); }
	Vector3 getOrientation(void) { return Vector3(m_yaw, m_pitch, m_roll); }
	Matrix4 getModelviewMatrix();

	// other getters and setters
	Scene*		getScene() { return m_scene; }
	bool		canBeRemoved() const { return m_canBeRemoved; }
	void		destroy() { m_canBeRemoved = true; }
	btRigidBody* getRigidBody() { return m_rigidBody; }
	void		setGraphNode(GraphNode* node) { m_graphNode = node; }
	GraphNode*	getGraphNode() { return m_graphNode; }
    bool		isInitialized() { return m_isInitialized; }
    Shader*		getShaderProgram() { return ShaderProgram; }
    void		setShaderProgram(Shader *shader) { ShaderProgram = shader; }
    string		getTexture0File() { return m_texture0File; }

	// rendering tests
	bool hasColors() { return (m_colors.size() > 0); }
	bool hasTexCoords() { return (m_texCoords.size() > 0); }
	bool hasNormals() { return (m_normals.size() > 0); }
    vector<Vector3>* getVertices() { return &m_vertices; }
    vector<Color>* getColors() { return &m_colors; }
    vector<TexCoord>* getTexCoords() { return &m_texCoords; }
    vector<Normal>* getNormals() { return &m_normals; }

protected:
	// implement these methods in subclasses (subclass lifecycle)
	virtual bool onInitialize() = 0;
	virtual void onPopulateVBOs() = 0;
	virtual void onPrepare(float dt) = 0;
	virtual void onRender() const = 0;
	virtual void onPostRender() = 0;
	virtual void onShutdown() = 0;

	// this is called only if you pass this entity to registerForSimulationTicks method in the scene that creates entity
	virtual void onSimulationTick(float dt) { }

	// VBOs and their vectors
	GLuint m_vertexVBO;
	std::vector<Vertex> m_vertices;
	GLuint m_colorVBO;
	std::vector<Color> m_colors;
	GLuint m_texCoordVBO;
	std::vector<TexCoord> m_texCoords;
	GLuint m_normalVBO;
	std::vector<Normal> m_normals;

	// shader
	Shader *ShaderProgram;

	// static texture map used to prevent images from being loaded multiple times across all entities
	static std::map<string, GLuint> s_textureMap;
    string m_texture0File;
	GLuint m_texture0;

	// position and orientation
	Vector3 m_position;
	float m_yaw;
	float m_pitch;
	float m_roll;

	// bullet
	btRigidBody *m_rigidBody;

	Scene* m_scene;
	bool m_canBeRemoved;
	GraphNode *m_graphNode;
    
    bool m_isInitialized;
};

#endif // ENTITY_H_INCLUDED
