
#ifndef __GUI_IMAGE__
#define __GUI_IMAGE__

#include "global.h"

class GUIImage
{
public:
    GUIImage();
    ~GUIImage();
    
    bool initialize(Shader *shader, const string& imageFile);
    void render();
    
    void setPosition(const Vector3& pos) { m_position = pos; }
    void setScale(const float& scale) { m_scale = scale; }
    
    bool isPointWithinImage(int x, int y);
    
private:
    Shader *ShaderProgram;
    TargaImage m_image;
    GLuint m_texID;
    
    GLuint m_vertexVBO;
	GLuint m_texCoordVBO;
    
    Vector3 m_position;
    float m_scale;
};

#endif 
