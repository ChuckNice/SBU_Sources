#pragma once
#include <stdafx.h>


class Game;
class InGameState;
class SpellButton;
class SpellDB;
class RenderList;
struct lua_State;
class TextBoxToDraw;
class SpellDBToolbar{
	int x, y, w,h;
	int btnWidth,btnHeight;
	vector<SpellButton*> btns;
	int hotPos,hotTimer;
	TextBoxToDraw* textBox;
	InGameState* state;
public:
	string currentSpell;
	int currentDown;
	SpellDBToolbar(int sX,int sY,int sW,int sH,int btnW,int btnH){
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
		currentSpell="";
	}

	//static int BUTTON_SOCKET;
	void initializeToolbar(InGameState* iGs, SpellDB *spellDB); 
	void handleMouse(InGameState* state,Game* g,int mx, int my);
	void render(RenderList* list);
	void handleClick(InGameState* state,Game* g, int mx, int my);
	void getSelectedSpell();
};