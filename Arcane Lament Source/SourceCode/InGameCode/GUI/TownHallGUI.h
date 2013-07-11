#pragma once
#include <stdafx.h>
#include <SourceCode\InGameCode\GUI\GUIScreenOverlay.h>
class Player;
class RenderList;
class Game;
class TownHallGUI:public GUIScreenOverlay{
	Player* p; 
	int popupOverlay;
	int sliderImage;
	bool dragSliderOne, dragSliderTwo;
	int slidersMin,slidersMax;
	int sliderOneX,sliderOneY;
	int sliderTwoX,sliderTwoY;
	wstring tstTxt,tstFont;
public:
	void setupForPlayer(Player* p);
	void handleMouse(int x, int y);
	void handleClick(int x, int y);
	void handleClickDrag(int xi, int yi,int xf,int yf);
	void dispatchClickDrag(int xi, int yi,int xf,int yf);
	void render(RenderList* list);
	void update(Game* g);
};