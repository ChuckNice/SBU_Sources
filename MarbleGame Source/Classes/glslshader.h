#ifndef GLSL_SHADER_H_INCLUDED
#define GLSL_SHADER_H_INCLUDED

#ifdef WIN32
    #include <windows.h>
    #include "glee/GLee.h"
#elif defined __APPLE__
    #ifndef MARBLE_IOS_BUILD
        #include <OpenGL/gl.h>
    #else
        #include <OpenGLES/ES2/gl.h>
    #endif
#endif

#include <map>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

using std::string;
using std::ifstream;
using std::map;
using std::vector;

class GLSLProgram
{
public:
    struct GLSLShader
    {
		GLSLShader() : id(0), linked(false) { }
        unsigned int id;
        string filename;
        string source;
		bool linked;
    };

    GLSLProgram(const string& vertexShader, const string& fragmentShader)
    {
		// use static maps to prevents shader source being read redundantly
		static map<string, GLSLShader> m_vertexShaderMap;
		static map<string, GLSLShader> m_fragmentShaderMap;
		m_vertexShader = &m_vertexShaderMap[vertexShader];
		m_fragmentShader = &m_fragmentShaderMap[fragmentShader];

        m_vertexShader->filename = vertexShader;
        m_fragmentShader->filename = fragmentShader;
    }

	virtual ~GLSLProgram()
	{
		unload();
	}

	void unload()
	{
		// NRTODO: very small memory leak since we can't delete these shader objects (they are shared across shared shaders)
		//glDetachShader(m_programID, m_vertexShader->id);
		//glDetachShader(m_programID, m_fragmentShader->id);
		//glDeleteShader(m_vertexShader.id);
		//glDeleteShader(m_fragmentShader.id);
		//glDeleteProgram(m_programID);
	}

	bool initialize()
	{
		static map<GLSLShader*, GLuint> m_programMap;
		m_programID = m_programMap[m_vertexShader];

		if ( m_vertexShader->id == 0 || m_fragmentShader->id == 0)
		{
			m_programID = glCreateProgram();

			m_vertexShader->id = glCreateShader(GL_VERTEX_SHADER);
			m_fragmentShader->id = glCreateShader(GL_FRAGMENT_SHADER);

			m_vertexShader->source = readFile(m_vertexShader->filename);
			m_fragmentShader->source = readFile(m_fragmentShader->filename);

			if (m_vertexShader->source.empty() || m_fragmentShader->source.empty())
			{
				return false;
			}

			const GLchar* tmp = static_cast<const GLchar*>(m_vertexShader->source.c_str());
			glShaderSource(m_vertexShader->id, 1, (const GLchar**)&tmp, NULL);

			tmp = static_cast<const GLchar*>(m_fragmentShader->source.c_str());
			glShaderSource(m_fragmentShader->id, 1, (const GLchar**)&tmp, NULL);

			if (!compileShader(*m_vertexShader) || !compileShader(*m_fragmentShader))
			{
				std::cerr << "Could not compile the shaders, they are invalid" << std::endl;
				return false;
			}

			glAttachShader(m_programID, m_vertexShader->id);
			glAttachShader(m_programID, m_fragmentShader->id);

			glLinkProgram(m_programID);

			m_programMap[m_vertexShader] = m_programID;
		}
		return true;
	}

	void linkProgram()
	{
		if (!m_vertexShader->linked)
		{
			glLinkProgram(m_programID);

			GLint result;
			glGetProgramiv(m_programID, GL_LINK_STATUS, &result);

			if (result == GL_FALSE) {
				outputProgramLog(m_programID);
				throw std::runtime_error("GLSL program did not link successfully, please check the logs");
			}

			m_vertexShader->linked = true;
		}
	}

    GLuint getUniformLocation(const string& name)
    {
        map<string, GLuint>::iterator i = m_uniformMap.find(name);
        if (i == m_uniformMap.end())
        {
            GLuint location = glGetUniformLocation(m_programID, name.c_str());
            m_uniformMap.insert(std::make_pair(name, location));
            return location;
        }

        return (*i).second;
    }

    GLuint getAttribLocation(const string& name)
    {
        map<string, GLuint>::iterator i = m_attribMap.find(name);
        if (i == m_attribMap.end())
        {
            GLuint location = glGetAttribLocation(m_programID, name.c_str());
            m_attribMap.insert(std::make_pair(name, location));
            return location;
        }

        return (*i).second;
    }

    void sendUniform(const string& name, const int id)
    {
        GLuint location = getUniformLocation(name);
        glUniform1i(location, id);
    }

    void sendUniform4x4(const string& name, const float* matrix, bool transpose=false)
    {
        GLuint location = getUniformLocation(name);
        glUniformMatrix4fv(location, 1, transpose, matrix);
    }

    void sendUniform3x3(const string& name, const float* matrix, bool transpose=false)
    {
        GLuint location = getUniformLocation(name);
        glUniformMatrix3fv(location, 1, transpose, matrix);
    }

    void sendUniform(const string& name, const float red, const float green,
                     const float blue, const float alpha)
    {
        GLuint location = getUniformLocation(name);
        glUniform4f(location, red, green, blue, alpha);
    }

	void sendUniform(const string& name, const float scalar)
    {
        GLuint location = getUniformLocation(name);
        glUniform1f(location, scalar);
    }

	void sendUniform(const string& name, const float x, const float y)
    {
        GLuint location = getUniformLocation(name);
        glUniform2f(location, x, y);
    }


    void sendUniform(const string& name, const float x, const float y,
                     const float z)
    {
        GLuint location = getUniformLocation(name);
        glUniform3f(location, x, y, z);
    }

    void bindAttrib(unsigned int index, const string& attribName)
    {
		if (!m_vertexShader->linked) {
			glBindAttribLocation(m_programID, index, attribName.c_str());
		}
    }

   virtual void bindShader()
    {
        glUseProgram(m_programID);
    }

    static bool glsl130Supported()
    {
        std::string version = (char*)glGetString(GL_SHADING_LANGUAGE_VERSION);
        if (version.find("1.30") != string::npos)
        {
            return true;
        }
        return false;
    }

private:
    string readFile(const string& filename)
    {
        ifstream fileIn(filename.c_str());

        if (!fileIn.good())
        {
            std::cerr << "Could not load shader: " << filename << std::endl;
            return string();
        }

        string stringBuffer(std::istreambuf_iterator<char>(fileIn), (std::istreambuf_iterator<char>()));
        return stringBuffer;
    }

    bool compileShader(const GLSLShader& shader)
    {
        glCompileShader(shader.id);
        GLint result = 0xDEADBEEF;
        glGetShaderiv(shader.id, GL_COMPILE_STATUS, &result);

        if (!result)
        {
            std::cout << "Could not compile shader: " << shader.id << std::endl;
            outputShaderLog(shader.id);
            return false;
        }

        return true;
    }

    void outputShaderLog(unsigned int shaderID)
    {
        vector<char> infoLog;
        GLint infoLen;
		glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLen);
		infoLog.resize(infoLen);

        std::cerr << "GLSL Shader: Shader contains errors, please validate this shader!" << std::endl;
        glGetShaderInfoLog(shaderID, (int)infoLog.size(), &infoLen, &infoLog[0]);

        std::cerr << string(infoLog.begin(), infoLog.end()) << std::endl;
#ifdef _WIN32
        MessageBox(NULL, (LPCSTR)string(infoLog.begin(), infoLog.end()).c_str(), "Error", MB_OK);
#elif defined __APPLE__
        printf("%s", string(infoLog.begin(), infoLog.end()).c_str());
#endif
    }

    void outputProgramLog(unsigned int programID)
    {
        vector<char> infoLog;
        GLint infoLen;
        GLint result;
        glGetProgramiv(programID, GL_LINK_STATUS, &result);

		if (result == GL_TRUE) {
		    return;
		}

		glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &infoLen);

		infoLog.resize(infoLen);

        std::cerr << "GLSL Shader: Shader contains errors, please validate this shader!" << std::endl;
        glGetProgramInfoLog(programID, (int)infoLog.size(), &infoLen, &infoLog[0]);

        std::cerr << string(infoLog.begin(), infoLog.end()) << std::endl;
#ifdef _WIN32
        MessageBox(NULL, (LPCSTR)string(infoLog.begin(), infoLog.end()).c_str(), "Error", MB_OK);
#elif defined __APPLE__
        printf("%s", string(infoLog.begin(), infoLog.end()).c_str());
#endif

    }

	GLSLShader *m_vertexShader;
	GLSLShader *m_fragmentShader;
    GLuint m_programID;

    map<string, GLuint> m_uniformMap;
    map<string, GLuint> m_attribMap;
};

#endif // GLSL_SHADER_H_INCLUDED
