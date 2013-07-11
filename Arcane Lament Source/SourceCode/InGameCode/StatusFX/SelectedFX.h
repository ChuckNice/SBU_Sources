#pragma once
#include "SourceCode\InGameCode\StatusFX.h"
class Unit;
class InGameState;
class SelectedFX:public StatusFX{
public:
	static int displayID,textureW,textureH;
	static StatusFactory<SelectedFX> factory;
	int index, *gsIndex;
	float sX,sY;
	Unit* me;
	InGameState* state;
	SelectedFX(){}
	~SelectedFX(){}
	void birth(Game* g);
	void applyToUnit(Unit* u);
	void render(RenderList* list, int x,int y);
	inline bool isDestroyed(){return index!=*gsIndex;}
	void release();
	bool stack(StatusFX* fx){return true;}
};