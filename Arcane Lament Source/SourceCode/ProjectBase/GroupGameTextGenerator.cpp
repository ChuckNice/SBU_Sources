/*
	Author: Angad Sidhu
*/

#include "stdafx.h"
#include "SourceCode\ProjectBase\GroupGameData.h"
#include "SourceCode\ProjectBase\GroupGameTextGenerator.h"
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\graphics\GameGraphics.h"
#include "SSSF_SourceCode\text\GameText.h"
#include "SSSF_SourceCode\timer\GameTimer.h"
#include "SSSF_SourceCode\PlatformPlugins\WindowsPlugin\WindowsTimer.h"

static const wstring	GG_TARGET_FPS_TEXT = L"FPS: ";
static const int		GG_TARGET_FPS_X	= 200;
static const int		GG_TARGET_FPS_Y	= 100;

/*
	appendTargetFPS - this method gets and appends the target frame rate to
	the text we are generating.
*/
void GroupGameTextGenerator::appendTargetFPS(Game *game)
{
	// WE'LL BUILD OUR WIDE STRING USING A WIDE STRING STREAM
	wstringstream wss;
	WindowsTimer *timer = (WindowsTimer*)game->getTimer();
	unsigned int time =timer->getLoopTime();

	// BUILD THE WIDE STRING
	textToGenerate.append(GG_TARGET_FPS_TEXT);
	fps*=.95f;
	if(time!=0)
		if(time>1000.0/60)
			fps+=1000.0f/(float)time*.05f;
		else 
			fps+=60*.05f;
	wss << fps;
	wss << L" BODIES:";
	wss << game->getGSM()->getPhysics()->getWorld()->GetBodyCount();
	textToGenerate.append(wss.str());
}
/*
	initText - Provides an example of how to render text to our screen every frame,
	allowing for the text to continually change. This method adds a text object
	to the text manager (GameText), which allows it to be updated each frame. This
	method should be called only once for each object.
*/
void GroupGameTextGenerator::initText(Game *game)
{
	// FIRST UPDATE THE TEXT THIS GENERATOR IS USING
	appendTargetFPS(game);
	// AND THEN HAND IT TO THE TEXT MANAGER, SPECIFYING WHERE IT SHOULD BE RENDERED
	GameText *text = game->getText();
	//text->addText(&textToGenerate, GG_TARGET_FPS_X, GG_TARGET_FPS_Y, 300,1000);//game->getGraphics()->getScreenWidth(), game->getGraphics()->getScreenHeight());
}

/*
	updateText - updates the text. Note that the text must already have been added
	to the text manager via the initText method. Also not that there should be a
	separate TextGenerator for each line of text we wish to render.
*/
void GroupGameTextGenerator::updateText(Game *game)
{
	// THE TEXT MANAGER (GameText) HAS A POINTER TO OUR TEXT, SO WE CAN
	// SIMPLY CLEAR IT OUT OF OLD CHARACTER DATA AND FILL IT IN WITH
	// WHAT WE WANT.
	textToGenerate.clear();
	appendTargetFPS(game);
}