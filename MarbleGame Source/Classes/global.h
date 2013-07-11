#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#ifdef _WIN32
    #include <windows.h>
    #include <glee/GLee.h>
    #include <GL/glu.h>
	#include "windows/glwindow.h"
#elif defined __APPLE__
    #ifndef MARBLE_IOS_BUILD
        #include <OpenGL/OpenGL.h>
    #else
        #include <OpenGLES/ES2/gl.h>
        #include <OpenGLES/ES2/glext.h>
    #endif
#endif

#include <ctime>
#include <stdio.h>
#include <iostream>
#include <vector>
#include <sstream>

#include <kazmath/kazmath.h>

#include "shaders.h"
#include "keyboard.h"
#include "mouse.h"
#include "geom.h"
#include "freetypefont.h"
#include "targa.h"
#include "camera.h"
#include "lighting.h"
#include "entity.h"
#include "scene_manager.h"

using namespace std;

// helper macro for printing strings to the debug window
#ifdef _WIN32
    #define GLOG(debugString) (OutputDebugString(##debugString))
#elif defined __APPLE__
    #define GLOG(debugString) printf(debugString)
#endif

// healper macros for random numbers
#define RAND(n) rand() % n
#define RANDF(n) (float(rand() % 1000) / 1000.0f) * n
#define RAND_COLOR Color(RANDF(1.0f), RANDF(1.0f), RANDF(1.0f))
#define RAND_LIGHT_COLOR Color(0.5f+RANDF(0.5f), 0.5f+RANDF(0.5f), 0.5f+RANDF(0.5f))
#define RAND_NEG1_1 (RANDF(2.0f) - 1.0f)

/*
This class is a singleton that manages global varibales
which don't really belong in a specific place. You can also add
convenient header includes, static things, and definitions in this
file to make them easily accessible to the whole program
*/
class Global
{
public:
	Global();
};

// external reference to global singleton
extern Global* s_Global();

#endif