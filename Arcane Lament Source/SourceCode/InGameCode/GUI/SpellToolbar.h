#pragma once
#include "stdafx.h"
#include <SourceCode\InGameCode\GUI\SpellButton.h>
#include <SourceCode\InGameCode\Spells\Spell.h>

class Game;
class RenderList;
struct lua_State;
class GameText;
class Wizard;
//class SpellButton;
//class TextBoxToDraw;
class SpellToolbar{
	unsigned int currentDown;
	int x, y, w,h;
	int hotPos, hotTimer;
	int btnWidth,btnHeight;
	vector<SpellButton*> btns;
	vector<Spell*> spells;
	TextBoxToDraw* textBox;
	Wizard* wizard;

public:
	SpellToolbar(){}
	SpellToolbar(int sX,int sY,int sW,int sH,int btnW,int btnH){
		x=sX;
		y=sY;
		w=sW;
		h=sH;
		btnWidth=btnW;
		btnHeight=btnH;
		hotPos=-1;
		hotTimer=0;
		currentDown=0;
		btns.clear();
		spells.clear();
		textBox=NULL;
	}
	void	clearToolbar(){btns.clear();spells.clear();}
	void	resetCurrentDown(){currentDown=-1;}
	void	snapToToolbar(int *x,int *y);
	void	addSpell(SpellButton *btn, Spell *p, int pos);
	void	removeSpell(SpellButton *btn, Spell *p);

	string  getTitleAtPos(int pos){ return btns.at(pos)->title;}

	void	initializeToolbar(Game* g);
	void	handleMouse(Game* g,int mx, int my);
	void	render(RenderList* list);
	void	handleClick(Game* g, int mx, int my);

	int		getPosAtMouse(int mx, int my);

	int		getDown()				{	return currentDown;	}
	Spell*	getSpell(int pos)		{	return spells.at(pos);	}
	
	void removeSpellByPos(int pos){
		spells.erase(spells.begin()+pos);
		btns.erase(btns.begin()+pos);
	}

	////NOT USED
	//Spell* swapSpell(int place, SpellButton *btn, Spell *p){
	//	btns.assign(place, btn);
	//	Spell* spell=spells.at(place);
	//	spells.assign(place, p);
	//	return spell;
	//}

	void setDown(unsigned int d){
		if(d>=btns.size())return;
		currentDown=d;
	}

	Spell* getCurrentSpell(){
		if(currentDown>=spells.size())return NULL;
		return spells.at(currentDown);
	}

	Spell* getSpell(int mx, int my){
		if(mx<x||mx>x+w||my<y||my>y+h)return NULL;
		int dx = x-mx;
		int dy = y-my;
		dx/=btnWidth;
		dy/=btnHeight;
		return spells.at(dy*(w/btnWidth)+dx);
	}

};