#pragma once
#include <stdafx.h>
#include <SourceCode\InGameCode\GUI\GUIScreenOverlay.h>
class Player;
class RenderList;
class Game;
struct lua_State;
class TextBoxToDraw;
class Barracks;
class BarracksGUI:public GUIScreenOverlay{
	Player* p;
	int popupOverlay,sliderImage,lineImage,buildbtnImage,goldIcon,popIcon;
	int lineWidth;
	int offX,offY,distBetweenRows,col1Space,col2Space;
	int buildBtnX,buildBtnY,buildBtnW,buildBtnH;
	map<int,wstring> tmp;
	float buttonScale;
	int selected;
	int currentlyEditing;
	bool endSelf;
	int currentlyDisplaying;
	TextBoxToDraw* txtBox;
public:
	static Barracks* currentBarracks;
	void initialize(lua_State* s,Player*p);
	void handleMouse(int x, int y);
	void handleClick(int x, int y);
	void handleClickDrag(int xi, int yi,int xf,int yf);
	void dispatchClickDrag(int xi, int yi,int xf,int yf);
	void render(RenderList* list);
	void update(Game* g);
	void onEnd();
	void issueOrder();
	void onStart();
	void clearSelf();
};