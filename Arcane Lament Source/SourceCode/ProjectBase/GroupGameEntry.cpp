/*
	Author: Angad Sidhu

 */
#pragma once
#include "stdafx.h"

//Luabind release mode
#define NDEBUG

//  GAME INCLUDES
#include "SourceCode\ProjectBase\GroupGameButtonEventHandler.h"
#include "SourceCode\ProjectBase\GroupGameData.h"
#include "SourceCode\ProjectBase\GroupGameDataLoader.h"
#include "SourceCode\ProjectBase\GroupGameKeyEventHandler.h"
#include "SourceCode\ProjectBase\GroupGameTextGenerator.h"

// GAME OBJECT INCLUDES
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\graphics\GameGraphics.h"
#include "SSSF_SourceCode\gsm\state\GameState.h"
#include "SSSF_SourceCode\gui\Cursor.h"
#include "SSSF_SourceCode\gui\GameGUI.h"
#include "SSSF_SourceCode\gui\ScreenGUI.h"
#include "SSSF_SourceCode\input\GameInput.h"
#include "SSSF_SourceCode\os\GameOS.h"
#include "SSSF_SourceCode\text\GameText.h"

// WINDOWS PLATFORM INCLUDES
#include "SSSF_SourceCode\PlatformPlugins\WindowsPlugin\WindowsOS.h"
#include "SSSF_SourceCode\PlatformPlugins\WindowsPlugin\WindowsInput.h"
#include "SSSF_SourceCode\PlatformPlugins\WindowsPlugin\WindowsTimer.h"

// DIRECTX INCLUDES
#include "SSSF_SourceCode\PlatformPlugins\DirectXPlugin\DirectXGraphics.h"
#include "SSSF_SourceCode\PlatformPlugins\DirectXPlugin\DirectXTextureManager.h"

/*
	WinMain - This is the application's starting point. In this method we will construct a Game object, 
	then initialize all the platform-dependent technologies, then construct all the custom data for our 
	game, and then initialize the Game with	our custom data. We'll then start the game loop.
*/
int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
	// CREATE THE GAME
	Game *dummyGame = new Game();

	// FIRST WE'LL SETUP THE DATA LOADER, SINCE IT MAY NEED TO READ
	// IN DATA TO SETUP OTHER STUFF
	GroupGameDataLoader *dummyDataLoader = new GroupGameDataLoader();
	dummyDataLoader->initWinHandle(hInstance, nCmdShow);
	dummyGame->setDataLoader(dummyDataLoader);
	dummyDataLoader->loadGame(dummyGame, GG_INIT_FILE);
	
	// WHAT WE SHOULD BE DOING HERE IS LOADING THE GAME DATA FROM FILES. THIS
	// MEANS THE GUIS THEMSELVES AS WELL AS THE LEVELS. THAT WILL BE LEFT
	// FOR BECHMARK HWS. FOR NOW WE WILL JUST HARD CODE THE LOADING OF THE GUI

	// LOAD THE GUI STUFF, AGAIN, NOTE THAT THIS SHOULD REALLY
	// BE DONE FROM A FILE, NOT HARD CODED
	dummyDataLoader->loadGUI(dummyGame, L"");

	// SPECIFY WHO WILL HANDLE BUTTON EVENTS
	GroupGameButtonEventHandler *dummyButtonHandler = new GroupGameButtonEventHandler();
	GameGUI *gui = dummyGame->getGUI();
	gui->registerButtonEventHandler((ButtonEventHandler*)dummyButtonHandler);

	// SPECIFY WHO WILL HANDLE KEY EVENTS
	GroupGameKeyEventHandler *dummyKeyHandler = new GroupGameKeyEventHandler();
	dummyGame->getInput()->registerKeyHandler((KeyEventHandler*)dummyKeyHandler);

	// START THE GAME LOOP
	dummyGame->runGameLoop();

	// GAME'S OVER SHUTDOWN ALL THE STUFF WE CONSTRUCTED HERE
	delete (WindowsOS*)dummyGame->getOS();
	delete (WindowsInput*)dummyGame->getInput();
	delete (WindowsTimer*)dummyGame->getTimer();
	delete (DirectXGraphics*)dummyGame->getGraphics();
	delete (GroupGameTextGenerator*)dummyGame->getText()->getTextGenerator();
	delete dummyButtonHandler;
	delete dummyKeyHandler;
	delete dummyGame;

	// AND RETURN
	return 0;
}