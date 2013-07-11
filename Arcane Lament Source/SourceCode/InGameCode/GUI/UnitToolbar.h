#pragma once
#include "stdafx.h"
class Unit;
class InGameState;
class RenderList;
class Game;
struct lua_State;
class Player;
class TextBoxToDraw;
class UnitToolbar{
	int x, y, w,h;
	int sideY;
	int btnWidth,btnHeight;
	int hotPos,hotTimer;
	TextBoxToDraw* textBox;
public:
	int bgID;
	int currentDown;
	wstring displayStrs[10];
	wstring font;
	Player* p;
	UnitToolbar(int sX,int sY,int sW,int sH,int btnW,int btnH,int sideY){
		x=sX;
		y=sY;
		w=sW;
		h=sH;
		this->sideY=sideY;
		btnWidth=btnW;
		btnHeight=btnH;
		currentDown=-1;
		hotPos=-1;
		hotTimer=0;
		textBox=NULL;
		font=L"Ariel";
	}
	void initializeToolbar( InGameState* s); 
	void handleMouse(InGameState* state,Game* g,int mx, int my);
	void render(RenderList* list);
	void handleClick(InGameState* state,Game* g, int mx, int my);
};