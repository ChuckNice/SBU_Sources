//
//  main.m
//  marblegame
//
//  Created by nitrex88 on 11/13/12.
//  Copyright (c) 2012 nitrex88. All rights reserved.
//

#include "global.h"
#include "loading_scene.h"
#include "main_menu.h"
#include "file_scene.h"
#include "bullet3D/LinearMath/btQuickprof.h"
#include <unistd.h>
#include <SDL/SDL.h>


bool onRender();

float width = 1204;
float height = 768;
bool m_isFullscreen = false;


int main(int argc, char *argv[])
{
    // initialize SDL and create OpenGL window
    SDL_Init(SDL_INIT_EVERYTHING);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    int flags = SDL_OPENGL | SDL_HWSURFACE;
    if (m_isFullscreen)
        flags |= SDL_FULLSCREEN;
    
    SDL_Surface *display = SDL_SetVideoMode(m_isFullscreen ? 0 : width,
                                            m_isFullscreen ? 0 : height, 0, flags);
    if (display == NULL) {
        GLOG("ERROR: Could not create SDL video surface");
        SDL_Quit();
        exit(1);
    }
    
    // set initial camera size
    s_Cam()->onResize(display->w, display->h);
    
    // hack to set working directory to the source root (no need to copy resources then)
    chdir(SRC_ROOT);
    char * dir = getcwd(NULL, 0);
    printf("Current dir: %s\n", dir);
    
    // create and intialize a scene
    if ( !s_SceneManager()->loadScene(new MainMenu()) ) {
    //if ( !s_SceneManager()->loadScene(new FileScene("Scenes/test.scene")) ) {
        printf("Could not initialize the scene");
        SDL_Quit();
        return 1;
    }
    
    // enter our main render loop
    while ( onRender() ) { }
    
    // cleanup
	s_SceneManager()->shutdown();
    SDL_Quit();
    
    return 0; //Return success
}


#pragma mark - Main Game Loop

SDL_Event event;
bool processEvents(float dt)
{
    // update keyboard before processing events (or we will miss key presses)
    s_Keyboard()->update();
    
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_VIDEORESIZE:
                s_Cam()->onResize(event.resize.w, event.resize.h);
                break;
                
            case SDL_KEYDOWN: {
                SDLKey key = event.key.keysym.sym;
                if (key == SDLK_ESCAPE) { return false; }
                s_Keyboard()->handleKeyDown(toupper(key));
                break; }
                
            case SDL_KEYUP:
                s_Keyboard()->handleKeyUp(toupper(event.key.keysym.sym));
                break;
        }
    }
    
    // update mouse
    s_Mouse()->update(dt);
    
    return true;
}


// variables used in render loop
float timeDelta = 1.0f / 480.0f;
float timeAccumulator = 0.0f;
float msec_sec = 1.0f / 1000.0f;
btClock bulletClock;
float prev_time = -1.0f;

bool onRender()
{
    // accumulate time from last frame and store new previous time
    if (prev_time > 0.0f)
        timeAccumulator += float(bulletClock.getTimeMilliseconds()) * msec_sec - prev_time;
    prev_time = float(bulletClock.getTimeMilliseconds()) * msec_sec;
    
    // step physics simulation with fixed timestep
    float timeSimulatedThisIteration = 0.0f;
    while ( timeAccumulator >= timeDelta )
    {
        s_SceneManager()->stepPhysics(timeDelta);
        timeAccumulator -= timeDelta;
        timeSimulatedThisIteration += timeDelta;
    }
    
    // update and render our scene with variable timestep
    s_SceneManager()->update(timeSimulatedThisIteration);
    s_SceneManager()->render();
    
    // process events such as keyboard and mouse
    bool success = processEvents(timeSimulatedThisIteration);
    
    // swap buffers!
    SDL_GL_SwapBuffers();
    
    return success;
}

