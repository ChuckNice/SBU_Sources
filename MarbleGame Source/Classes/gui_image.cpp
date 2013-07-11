
#include "gui_image.h"


GUIImage::GUIImage() :
m_texID(0),
m_position(Vector3(0.0f, 0.0f, 0.0f))
{
}

GUIImage::~GUIImage()
{
    glDeleteBuffers(1, &m_vertexVBO);
    glDeleteBuffers(1, &m_texCoordVBO);
    glDeleteTextures(1, &m_texID);
    if (ShaderProgram != NULL)
		delete ShaderProgram;
}


bool GUIImage::initialize(Shader *shader, const string& imageFile)
{
    ShaderProgram = shader;
    
    if ( !m_image.load(imageFile) )
        return false;
    glGenTextures(1, &m_texID);
    glBindTexture(GL_TEXTURE_2D, m_texID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
#ifndef MARBLE_IOS_BUILD
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB8, m_image.getWidth(), m_image.getHeight(), GL_RGB, GL_UNSIGNED_BYTE, m_image.getImageData());
#else
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_image.getWidth(), m_image.getHeight(), 0, GL_RGB, GL_UNSIGNED_BYTE, m_image.getImageData());
#endif
    
    // send vertices
    float width = m_image.getWidth() * 0.5f;
    float height = m_image.getHeight() * 0.5f;
    Vector3 vertices[4] = {
        Vector3(-width, -height, 0.0f),
        Vector3(-width,  height, 0.0f),
        Vector3( width, -height, 0.0f),
        Vector3( width,  height, 0.0f)
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
    
    return true;
}


void GUIImage::render()
{
    ShaderProgram->bindShader();
    
	// send vertices to shader
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexVBO);
    glVertexAttribPointer((GLint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    
	// send texcoords to shader
	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordVBO);
    glVertexAttribPointer((GLint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    
    glEnable(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_texID);
    
	// calculate the modelview for this image
	Matrix4 modelview;
	modelview.translatef(m_position.x, m_position.y, 0.0f);
    modelview.scalef(m_scale);
    
	// send matrices to shader program
	ShaderProgram->sendUniform4x4("modelview_matrix", modelview.mat);
	ShaderProgram->sendUniform4x4("projection_matrix", s_Cam()->orthoProjectionMatrix()->mat);
    
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
	// cleanup
    glDisable(GL_TEXTURE_2D);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(2);
	glUseProgram(0);
}



bool GUIImage::isPointWithinImage(int x, int y)
{
    int left = (int)(m_position.x - (m_image.getWidth() * 0.5f * m_scale));
    int right = (int)(m_position.x + (m_image.getWidth() * 0.5f * m_scale));
    int bottom = (int)(m_position.y - (m_image.getHeight() * 0.5f * m_scale));
    int top = (int)(m_position.y + (m_image.getHeight() * 0.5f * m_scale));
    
    if (x >= left && x <= right &&
        y >= bottom && y <= top)
        return true;
    
    return false;
}








