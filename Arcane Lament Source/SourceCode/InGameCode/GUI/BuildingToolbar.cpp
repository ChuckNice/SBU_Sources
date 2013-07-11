#include <SourceCode\InGameCode\GUI\BuildingToolbar.h>
#include <SourceCode\ProjectBase\ResourceBank.h>
#include <SourceCode\InGameCode\Building.h>
#include <SourceCode\InGameCode\Units\Building\Barracks.h>
#include <SourceCode\InGameCode\GUI\BuildingButton.h>
//#include "luabind\luabind.hpp"
//#include "lua.h"
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\graphics\RenderList.h>
#include "stdafx.h"
#include <SSSF_SourceCode\stdutil.h>
#include <SourceCode\InGameCode\InGameState.h>
#include "SSSF_SourceCode\text\GameText.h"
#include "SSSF_SourceCode\text\TextBoxToDraw.h"
#include <SSSF_SourceCode\sound\SoundManager.h>

void BuildingToolbar::initializeToolbar(Game* g,lua_State* loadIn, InGameState* s){
		/**
			lua table should be constructed like follows
			table of buttons
			each button has
			Xstring defining the type
			string pointing to button up, button down
			Xstring pointing to sprite type
			string pointing to button title, button tooltip
			string pointing to button title font, button tooltip font
			string pointing to button title color 4, button tooltip color 4
		**/
		luabind::object toolbarTable = luabind::globals(loadIn)["BuildingToolBarDef"];
		for(luabind::iterator i(toolbarTable),end;i!=end;i++){
			BuildingButton b;
			int j=1;
			b.upImg=ResourceBankgetResource(luabind::object_cast<string>(i->operator[](j++)));//GetUP
			b.downImg=ResourceBankgetResource(luabind::object_cast<string>(i->operator[](j++)));//GetDOWN
			b.title=(luabind::object_cast<string>(i->operator[](j++)));//GETTITLE
			b.tooltip=(luabind::object_cast<string>(i->operator[](j++)));//GetToolTip
			b.titleFont=(luabind::object_cast<string>(i->operator[](j++)));//GetTitleFont
			b.tooltipFont=(luabind::object_cast<string>(i->operator[](j++)));//GetTitleFont
			b.titleColor[0]=(luabind::object_cast<int>(i->operator[](j++)));
			b.titleColor[1]=(luabind::object_cast<int>(i->operator[](j++)));
			b.titleColor[2]=(luabind::object_cast<int>(i->operator[](j++)));
			b.titleColor[3]=(luabind::object_cast<int>(i->operator[](j++)));
			b.tooltipColor[0]=(luabind::object_cast<int>(i->operator[](j++)));
			b.tooltipColor[1]=(luabind::object_cast<int>(i->operator[](j++)));
			b.tooltipColor[2]=(luabind::object_cast<int>(i->operator[](j++)));
			b.tooltipColor[3]=(luabind::object_cast<int>(i->operator[](j++)));
			b.backgroundID=ResourceBankgetResource(luabind::object_cast<string>(i->operator[](j++)));//GetBG
			b.type=convertwstrX(luabind::object_cast<string>(i->operator[](j++)));
			btns.push_back(make_pair(b,(Building*)g->getGSM()->getSpriteManager()->getBotRecycler()->retrieveBot(b.type)));
		}
	}


void BuildingToolbar::handleMouse(InGameState* state,Game* g,int mx, int my){
	for(int i=0;i<btns.size();i++)
		btns.at(i).first.canBuild=btns.at(i).second->canBuild(g,state,state->mainPlayer);
	if(mx<x||mx>x+w||my<y||my>y+h){
		hotPos=-1;
		if(textBox!=NULL){
			textBox->release();
			textBox=NULL;
		}
		return;
	}
	int dx = mx-x;
	int dy = my-y;
	dx/=btnWidth;
	dy/=btnHeight;
	unsigned int pos =(dy*(w/btnWidth)+dx);
	if(pos<btns.size()){
		if(hotPos!=pos){
			hotPos=pos;
			hotTimer=0;
			if(textBox!=NULL){
				textBox->release();
				textBox=NULL;
			}
			BuildingButton b = btns.at(pos).first;
			textBox=g->createTextBoxSimple(b.title,"",mx,my-40,200);
			g->setTitle(textBox,b.title,b.titleFont,18,b.titleColor[0],b.titleColor[1],b.titleColor[2],b.titleColor[3]);
			g->setBG(textBox,b.backgroundID,10,5);
		}
		else hotTimer++;
		if(hotTimer==24){
			textBox->release();
			BuildingButton b = btns.at(pos).first;
			textBox=g->createTextBoxSimple(b.title,b.tooltip,textBox->x,textBox->y-60,200);
			g->setTitle(textBox,b.title,b.titleFont,18,b.titleColor[0],b.titleColor[1],b.titleColor[2],b.titleColor[3]);
			g->setBody(textBox,b.tooltip,b.tooltipFont,16,b.tooltipColor[0],b.tooltipColor[1],b.tooltipColor[2],b.tooltipColor[3]);
			g->setBG(textBox,b.backgroundID,10,5);
		}
	}
	else{ 
		hotPos=-1;
		if(textBox!=NULL){
			textBox->release();
			textBox=NULL;
		}
	}
	
}
void BuildingToolbar::render(RenderList* list){
	int wBtns = w/btnWidth;
	for(int i=0;i<btns.size();i++){
		BuildingButton b= btns.at(i).first;
		if(b.canBuild)list->addRenderItem(b.upImg,
			x+(i%wBtns)*btnWidth,
			y+i/wBtns*btnHeight,
			0,
			255,
			btnWidth,
			btnHeight
			);
		else list->addRenderItem(b.downImg,
			x+(i%wBtns)*btnWidth,
			y+i/wBtns*btnHeight,
			0,
			255,
			btnWidth,
			btnHeight
			);
	}
}
void BuildingToolbar::handleClick(InGameState* state,Game* g, int mx, int my){
	if(mx<x||mx>x+w||my<y||my>y+h)return;
	int dx = mx-x;
	int dy = my-y;
	dx/=btnWidth;
	dy/=btnHeight;
	int pos =(dy*(w/btnWidth)+dx);
	if(pos<btns.size() && btns.at(pos).first.canBuild){
		currentDown=pos;
		//g->getSoundManager()->playCue("VICTORY");
		state->createNewBuilding((Building*)btns.at(pos).second->clone(g));
	}
}