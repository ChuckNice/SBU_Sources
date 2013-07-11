#pragma once
#include "stdafx.h"

/*

	This file contains setup data for making the game.
*/

// HERE IS THE GAME SETUP/INIT FILE. EVERYTHING WOULD START FROM HERE
static const wstring	GG_INIT_FILE			= L"data/GameSetup.txt";
static const int TILE_WIDTH = 32;
static const int TILE_HEIGHT = 32;

// HERE ARE THE GAME PROPERTIES WE ARE LOADING FROM FROM THE SETUP FILE
static const wstring	GG_TITLE				= L"TITLE";
static const wstring	GG_SCREEN_WIDTH			= L"SCREEN_WIDTH";
static const wstring	GG_SCREEN_HEIGHT		= L"SCREEN_HEIGHT";
static const wstring	GG_USE_FULLSCREEN_MODE	= L"USE_FULLSCREEN_MODE";
static const wstring	GG_TEXT_FONT_SIZE		= L"TEXT_FONT_SIZE";

//Probably not going to be necessary
static const wstring	GG_VIEWPORT_OFFSET_X	= L"VIEWPORT_OFFSET_X";
static const wstring	GG_VIEWPORT_OFFSET_Y	= L"VIEWPORT_OFFSET_Y";
static const wstring	GG_VIEWPORT_BOTTOM_OFFSET_X	= L"VIEWPORT_BOTTOM_OFFSET_X";
static const wstring	GG_VIEWPORT_BOTTOM_OFFSET_Y	= L"VIEWPORT_BOTTOM_OFFSET_Y";

//When these are loaded from file its not going to be constant
static const wstring	GG_FONT_COLOR_RED		= L"FONT_COLOR_RED";
static const wstring	GG_FONT_COLOR_GREEN		= L"FONT_COLOR_GREEN";
static const wstring	GG_FONT_COLOR_BLUE		= L"FONT_COLOR_BLUE";

static const wstring DEATH_STATE=L"DEATH_STATE";

//This is going to be ported to a GUI LUA system
//GUI Button Commands
static const wstring GG_GO_TO_MM_COMMAND	= L"Main Menu";
static const wstring GG_START_COMMAND		= L"Start Game";
static const wstring GG_EXIT_COMMAND		= L"Exit";
static const wstring GG_QUIT_COMMAND		= L"Quit";
static const wstring GG_CONTROLS_COMMAND	= L"Controls";
static const wstring GG_ABOUT_COMMAND		= L"AboutMe";
static const wstring GG_PAUSEINGAME_COMMAND = L"Pause";
static const wstring GG_RESUME_COMMAND = L"Resume";
static const wstring GG_INGAMECTRLS_COMMAND = L"InGameControls";

//Box2D length Scale factor
static const float BOX2D_Scale_Factor=1.0f/15.0f;
static const float BOX2D_Scale_Factor_INV=15.0f;

//ACTOR HOOK TYPES
static const int ROCK_TYPE=0;
static const int UNIT_TYPE=1<<4;
static const int BUILDING_TYPE=1<<2;
static const int SPELL_TYPE=1<<3;


//Warrior find target range
static const float warriorFindTargetRange=150;
static const float warriorAttackTargetRange=20;
// OGRE TARGET RANGES
static const float ogreFindTargetRange=200;
static const float ogreAttackTargetRange=50;//delete

//This is for collision filtering
static const unsigned int tinyUnitCategory=0x02;
static const unsigned int tinyUnitFilter=0xFD;

//FX ID's
static const wstring UNITHPBARFILLEDID=L"UnitHPBar";

//ORDER CMDS
static const int STOPORDER=0;
static const int MOVEORDER=1;
static const int ATTACKTOORDER=2;
static const int ATTACKUNITORDER=4;
static const int PATROLTOORDER=3;

static const int TUTSYSSCREENOVERLAYNUM=29;
static const int TOWNHALLGUISCREENOVERLAYNUM=25;
static const int BARRACKSGUISCREENOVERLAYNUM=26;
static const int SPELL_SELECTION_GUI=10;
static const int STATS_SCREEN=38;


inline bool isOfTypeUnit(int type){
	return type==UNIT_TYPE||type==BUILDING_TYPE;
}