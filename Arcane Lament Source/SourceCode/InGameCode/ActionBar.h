#pragma once

#include <vector>
#include <SourceCode\InGameCode\ActionButton.h>

#define NUMTYPEOFBUILDINGS 1
class Building;
class InGameState;
class RenderList;
class Game;
struct lua_State;
class TextBoxToDraw;
class ActionBar{
	int effectiveX,effectiveY;
	int offTime;
	int x, y, w,h;
	int btnWidth,btnHeight;
	vector<ActionButton> btns;
	int hotPos,hotTimer;
	TextBoxToDraw* textBox;
	bool display, opened;
public:
	int currentDown;
	ActionBar(int sX,int sY,int sW,int sH,int btnW,int btnH){
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
		display=opened=false;
		offTime=0;
	}
	void openAt(Game* g,int x, int y);
	void initializeToolbar(Game* g,lua_State* loadIn); 
	void handleMouse(InGameState* state,Game* g,int mx, int my);
	void render(RenderList* list);
	bool handleClick(InGameState* state,Game* g, int mx, int my,int which);
};