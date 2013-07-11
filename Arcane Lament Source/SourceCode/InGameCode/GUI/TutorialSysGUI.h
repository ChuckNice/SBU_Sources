#pragma once
#include <stdafx.h>
#include <sourcecode\ingamecode\gui\guiscreenoverlay.h>
struct lua_State;
class InGameState;
class Game;
class RenderList;
class TextBoxToDraw;
struct TutorialSysImage{
	int imgId, x,y;
};
struct TutorialSysText{
	int x,y,size,width;
	string text, font;
};
class TutorialSysGUI: public GUIScreenOverlay{
	vector<TextBoxToDraw*> toRelease;
	lua_State* state;
	int background;
	InGameState *iGs;
	Game *game;
	vector<TutorialSysImage> images;
	vector<TutorialSysText> textBoxes;
	int currentScreen;
public:
	string currentTutorial;
	void setupSys(InGameState* s,Game *g, lua_State *state);
	void loadScreen();
	void render(RenderList* list);
	void handleMouse(int x, int y){}
	void handleClick(int x, int y){currentScreen++;loadScreen();}
	void handleClickDrag(int xi, int yi,int xf,int yf){}
	void dispatchClickDrag(int xi, int yi,int xf,int yf){currentScreen++;loadScreen();}
	void update(Game* g){}
	void onStart(){
		currentScreen=1;
		loadScreen();
	}
	void onEnd();
};