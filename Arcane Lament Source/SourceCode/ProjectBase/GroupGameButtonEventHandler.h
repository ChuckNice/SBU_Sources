/*	
	Author: Angad Sidhu

*/

#pragma once
#include "stdafx.h"
#include "SSSF_SourceCode\input\ButtonEventHandler.h"

class Game;

class GroupGameButtonEventHandler: public ButtonEventHandler
{
public:
	// AS-IS, THIS CLASS HAS NO DATA, SO THERE IS NOTHING
	// TO INITIALIZE OR DESTROY
	GroupGameButtonEventHandler()	{}
	~GroupGameButtonEventHandler()	{}

	// DEFINED INSIDE GroupGameButtonEventHandler.cpp
	void handleButtonEvents(Game *game, 
							wstring command);
};