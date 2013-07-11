#pragma once

class Player;
class RenderList;
class Game;
class GUIScreenOverlay{
public:
	virtual void handleMouse(int x, int y)=0;
	virtual void handleClick(int x, int y)=0;
	virtual void handleClickDrag(int xi, int yi,int xf,int yf)=0;
	virtual void dispatchClickDrag(int xi, int yi,int xf,int yf)=0;
	virtual void render(RenderList* list)=0;
	virtual void update(Game* g)=0;
	virtual void onStart(){}
	virtual void onEnd(){}
};