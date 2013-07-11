#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN

#include "global.h"
#include "main_menu.h"
#include "file_scene.h"
#include "bullet3D/LinearMath/btQuickprof.h"

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR cmdLine,
                   int cmdShow)
{
    //Set our window settings
    const int windowWidth = 1024;
    const int windowHeight = 768;
    const int windowBPP = 16;
    bool windowFullscreen = false;

    //This is our window
    GLWindow programWindow(hInstance);

    if (MessageBox(NULL, "Would you like to run in fullscreen mode?", "Fullscreen?", MB_ICONINFORMATION | MB_YESNO) == IDYES)
        windowFullscreen = true;

    //Attempt to create the window
    if (!programWindow.create(windowWidth, windowHeight, windowBPP, windowFullscreen))
    {
        //If it fails
        MessageBox(NULL, "Unable to create the OpenGL Window", "An error occurred", MB_ICONERROR | MB_OK);
        programWindow.destroy(); //Reset the display and exit
        return 1;
    }

	if ( !s_SceneManager()->loadScene(new MainMenu()) ) //Initialize our example
    //if ( !s_SceneManager()->loadScene(new FileScene("Scenes/test.scene")) )
	{
        MessageBox(NULL, "Could not initialize the scene", "An error occurred", MB_ICONERROR | MB_OK);

        programWindow.destroy(); //Reset the display and exit
        return 1;
    }

    //This is the mainloop, we render frames until isRunning returns false
	float timeDelta = 1.0f / 480.0f;
	float timeAccumulator = 0.0f;
	float msec_sec = 1.0f / 1000.0f;
	btClock clock;
    while( programWindow.isRunning() )
    {
		// this is for fixed timestep physics simulation with variable timestep rendering
		float timeSimulatedThisIteration = 0.0f;
		float startTime = float(clock.getTimeMilliseconds()) * msec_sec;

		while ( timeAccumulator >= timeDelta )
		{
			s_SceneManager()->stepPhysics(timeDelta);
			timeAccumulator -= timeDelta;
			timeSimulatedThisIteration += timeDelta;
		}

		s_SceneManager()->update(timeSimulatedThisIteration);
		s_SceneManager()->render();

		programWindow.processEvents(timeSimulatedThisIteration); //Process any window events

		timeAccumulator += float(clock.getTimeMilliseconds()) * msec_sec - startTime;


		//// uncomment this and comment out the above code for variable timestep physics
		//float elapsedTime = programWindow.getElapsedSeconds();

		//programWindow.processEvents(elapsedTime); //Process any window events

		//s_SceneManager()->stepPhysics(elapsedTime);
		//s_SceneManager()->update(elapsedTime);
		//s_SceneManager()->render();


		programWindow.swapBuffers();
	}

	// cleanup
	s_SceneManager()->shutdown();
    programWindow.destroy(); //Destroy the program window

    return 0; //Return success
}


