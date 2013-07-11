#ifndef __SHADER_H__
#define __SHADER_H__

#ifdef _WIN32
#include <windows.h>
#endif

#include "glslshader.h"
#include "geom.h"

class Shader : public GLSLProgram
{
public:
	Shader(string vertFile, string fragFile, bool use_color, bool use_texture, bool use_lighting) :
    GLSLProgram(vertFile, fragFile),
    m_supportsColoring(use_color), m_supportsTexturing(use_texture), m_supportsLighting(use_lighting),
    m_usesBlending(false)
    {
        // initialize our shader
        if ( !initialize() ) {
#ifdef _WIN32
            OutputDebugString("ERROR: Could not initialize shader\n");
#elif defined __APPLE__
            printf("ERROR: Could not initialize shader\n");
#endif
            throw;
        }
        bindAttrib(0, "a_Vertex");
        if ( supportsColoring() ) { bindAttrib(1, "a_Color"); }
        if ( supportsTexuring() ) { bindAttrib(2, "a_TexCoord0"); }
        if ( supportsLighting() ) { bindAttrib(3, "a_Normal"); }
        linkProgram();
        // send initial uniforms if we need to
        if (supportsTexuring()) { sendUniform("texture0", 0); }
    }
    
    bool supportsColoring() { return m_supportsColoring; }
    bool supportsTexuring() { return m_supportsTexturing; }
    bool supportsLighting() { return m_supportsLighting; }
    bool usesBlending() { return m_usesBlending; }
    virtual void update(float dt) { }
    
    
protected:
	bool m_supportsColoring;
	bool m_supportsTexturing;
	bool m_supportsLighting;
	bool m_usesBlending;
};


class ColorShader : public Shader
{
public:
	ColorShader() :
    Shader("Shaders/color.vert", "Shaders/color.frag", true, false, false) { }
};


class GlowShader : public Shader
{
public:
	Color glowColor;
	GlowShader(Color color) :
    Shader("Shaders/glow.vert", "Shaders/glow.frag", false, true, false),
    glowColor(color) { }
	void bindShader() {
		GLSLProgram::bindShader();
		sendUniform("color", glowColor.r, glowColor.g, glowColor.b, glowColor.a);
	}
};


class TextureShader : public Shader
{
public:
    Color color;
	TextureShader(Color col = Color(1.0f, 1.0f, 1.0f, 1.0f)) :
    Shader("Shaders/texture.vert", "Shaders/texture.frag", false, true, false), color(col) { }
    void bindShader() {
		GLSLProgram::bindShader();
		sendUniform("color", color.r, color.g, color.b, color.a);
	}
};


class LightShader : public Shader
{
public:
	float ambientFactor;
	float diffuseFactor;
	int specularPower;
	Color diffuseColor;
	LightShader(float ambient, float diffuse, int specular, Color color, bool blending = false) :
    Shader("Shaders/light.vert", "Shaders/light.frag", false, false, true),
    ambientFactor(ambient), diffuseFactor(diffuse), specularPower(specular), diffuseColor(color) {
        m_usesBlending = blending;
    }
	void bindShader() {
		GLSLProgram::bindShader();
		sendUniform("ambient", ambientFactor);
		sendUniform("diffuse", diffuseFactor);
		sendUniform("specular", specularPower);
		sendUniform("diffuseColor", diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
	}
};


class LightTexShader : public Shader
{
public:
	float ambientFactor;
	float diffuseFactor;
	int specularPower;
	Color diffuseColor;
	LightTexShader(float ambient, float diffuse, int specular, Color color, bool blending = false) :
    Shader("Shaders/light_tex.vert", "Shaders/light_tex.frag", false, true, true),
    ambientFactor(ambient), diffuseFactor(diffuse), specularPower(specular), diffuseColor(color) {
        m_usesBlending = blending;
    }
	void bindShader() {
		GLSLProgram::bindShader();
		sendUniform("ambient", ambientFactor);
		sendUniform("diffuse", diffuseFactor);
		sendUniform("specular", specularPower);
		sendUniform("diffuseColor", diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
	}
};


class LightTexScrollShader : public Shader
{
public:
	float ambientFactor;
	float diffuseFactor;
	int specularPower;
	Color diffuseColor;
    Vector3 scrollDirection;
    float scrollSpeed;
    float textureScale;
	LightTexScrollShader(float ambient, float diffuse, int specular, Color color, Vector3 direction, float speed, float scale = 1.0f, bool blending = false) :
    Shader("Shaders/light_tex_scroll.vert", "Shaders/light_tex_scroll.frag", false, true, true),
    ambientFactor(ambient), diffuseFactor(diffuse), specularPower(specular), diffuseColor(color),
    scrollDirection(direction), scrollSpeed(speed), textureScale(scale) {
        m_usesBlending = blending;
        m_translation = Vector3(0.0f, 0.0f, 0.0f);
        scrollDirection.normalize();
    }
	void bindShader() {
		GLSLProgram::bindShader();
		sendUniform("ambient", ambientFactor);
		sendUniform("diffuse", diffuseFactor);
		sendUniform("specular", specularPower);
		sendUniform("diffuseColor", diffuseColor.r, diffuseColor.g, diffuseColor.b, diffuseColor.a);
        Matrix4 texture_matrix;
        texture_matrix.translatef(m_translation.x, m_translation.y, m_translation.z);
        texture_matrix.scalef(textureScale);
        sendUniform4x4("texture_matrix", texture_matrix.mat);
	}
    void update(float dt) {
        // update the translation of the texture
        m_translation += scrollDirection * scrollSpeed * dt;
    }
private:
    Vector3 m_translation;
};


class TrampolineShader : public Shader
{
public:
	TrampolineShader() :
    Shader("Shaders/trampoline.vert", "Shaders/trampoline.frag", true, true, true) { }
};

#endif