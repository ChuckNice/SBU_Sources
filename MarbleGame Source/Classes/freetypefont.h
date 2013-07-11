#ifndef BOGLGP_FREETYPE_FONT_H
#define BOGLGP_FREETYPE_FONT_H


// FreeType Headers
#include <freetype/ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#include <string>
#include "glslshader.h"

class FreeTypeFont {
public:
    FreeTypeFont(const std::string& fontName, int screenWidth, int screenHeight, int fontSize=16,
                 const std::string& vertShader="data/font.vert", const std::string& fragShader="data/font.frag");
    ~FreeTypeFont();

    bool initialize();
    void printString(const std::string& str, float x, float y);

private:
    GLuint m_textureID[128]; //Store room for the character textures

    int m_fontSize;
    int m_screenWidth;
    int m_screenHeight;
    std::string m_fontName;

    GLuint m_texCoordBuffer;
    GLuint m_vertexBuffer;

    bool generateCharacterTexture(unsigned char ch, FT_Face fontInfo);

    GLSLProgram* m_shaderProgram;

    std::map<char, std::pair<int, int> > m_glyphDimensions;
    std::map<char, std::pair<int, int> > m_glyphPositions;
    std::map<char, int> m_glyphAdvances;
};

#endif
