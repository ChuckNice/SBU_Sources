/*	
	Author: Angad Sidhu
*/

#pragma once
#include "stdafx.h"
#include "SSSF_SourceCode\input\KeyEventHandler.h"

class Game;

class GroupGameKeyEventHandler : public KeyEventHandler
{
public:
	int xi, yi,screenxi,screenyi, xf, yf;
	bool mousePrevPressed;
	GroupGameKeyEventHandler()		{}
	~GroupGameKeyEventHandler()		{}
	void handleKeyEvents(Game *game);
};