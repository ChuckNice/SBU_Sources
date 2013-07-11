#pragma once
#include "stdafx.h"
#include <SourceCode\InGameCode\GUI\BuildingButton.h>

#define NUMTYPEOFBUILDINGS 1
class Building;
class InGameState;
class RenderList;
class Game;
struct lua_State;
class TextBoxToDraw;
class BuildingToolbar{
	int x, y, w,h;
	int btnWidth,btnHeight;
	vector<pair<BuildingButton,Building*>> btns;
	int hotPos,hotTimer;
	TextBoxToDraw* textBox;
public:
	int currentDown;
	BuildingToolbar(int sX,int sY,int sW,int sH,int btnW,int btnH){
		x=sX;
		y=sY;
		w=sW;
		h=sH;
		btnWidth=btnW;
		btnHeight=btnH;
		btns.clear();
		currentDown=-1;
		hotPos=-1;
		hotTimer=0;
		textBox=NULL;
	}
	void initializeToolbar(Game* g,lua_State* loadIn, InGameState* s); 
	void handleMouse(InGameState* state,Game* g,int mx, int my);
	void render(RenderList* list);
	void handleClick(InGameState* state,Game* g, int mx, int my);
};