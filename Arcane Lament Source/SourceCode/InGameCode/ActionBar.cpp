#include <SourceCode\InGameCode\ActionBar.h>
#include <SourceCode\ProjectBase\ResourceBank.h>
#include <SourceCode\InGameCode\Building.h>
#include <SourceCode\InGameCode\GUI\BuildingButton.h>
//#include "luabind\luabind.hpp"
//#include "lua.h"
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\graphics\RenderList.h>
#include "stdafx.h"
#include <SourceCode\InGameCode\InGameState.h>
#include "SSSF_SourceCode\text\GameText.h"
#include "SSSF_SourceCode\text\TextBoxToDraw.h"

void ActionBar::initializeToolbar(Game* g,lua_State* loadIn){
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
		luabind::object toolbarTable = luabind::globals(loadIn)["CommandToolBarDef"];
		for(luabind::iterator i(toolbarTable),end;i!=end;i++){
			ActionButton b;
			int j=1;
			b.upImg=ResourceBankgetResource(luabind::object_cast<string>(i->operator[](j++)));//GetUP
			b.cmd=(luabind::object_cast<string>(i->operator[](j++)));//GETCMD
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
			btns.push_back(b);
		}
	}

void ActionBar::handleMouse(InGameState* state,Game* g,int mx, int my){
	if(!opened)return;
	if(mx<x||mx>x+w||my<y||my>y+h){
		hotPos=-1;
		if(textBox!=NULL){
			textBox->release();
			textBox=NULL;
		}
		offTime--;
		if(offTime<=0){
			opened=false;
			display=false;
		}
		return;
	}
	offTime=20;
	int dx = mx-x;
	int dy = my-y;
	dx/=btnWidth;
	dy/=btnHeight;
	unsigned int pos =(dy*(w/btnWidth)+dx);
	if(pos<btns.size()){
		{
			if(textBox!=NULL){
				textBox->release();
				textBox=NULL;
			}
			ActionButton b = btns.at(pos);
			textBox=g->createTextBoxSimple(b.title,"",mx,my-40,200);
			g->setTitle(textBox,b.title,b.titleFont,16,b.titleColor[0],b.titleColor[1],b.titleColor[2],b.titleColor[3]);
			g->setBG(textBox,b.backgroundID,10,5);
		}
	}
	else{
		if(textBox!=NULL){
			textBox->release();
			textBox=NULL;
		}
	}
	
}
void ActionBar::render(RenderList* list){
	if(display){
		int wBtns = w/btnWidth;
		for(int i=0;i<btns.size();i++){
			ActionButton b= btns.at(i);
			list->addRenderItem(b.upImg,
				x+(i%wBtns)*btnWidth,
				y+i/wBtns*btnHeight,
				0,
				255,
				btnWidth,
				btnHeight
				);
		}
	}
}
class IsBuildingThereCB:public b2QueryCallback{
public:
	Unit** setup;
	b2Vec2 center;
	float min;
	bool ReportFixture(b2Fixture* fixture)
		{
			if(fixture->IsSensor())return true;
			b2Body* body = fixture->GetBody();
			ActorHook* data = (ActorHook*)body->GetUserData();
			if(data->type==BUILDING_TYPE){
				Unit* inRange = (Unit*)data->data;
				float len=(inRange->getBody()->GetPosition()-center).LengthSquared();
				if(len<min){
					*setup=inRange;
				}
			}
			return true;
		}

};

bool ActionBar::handleClick(InGameState* state,Game* g, int mx, int my,int which){
	Viewport *vp = g->getGUI()->getViewport();
	if(which!=0)return false;
	if(mx<x||mx>x+w||my<y||my>y+h)return false;
	int dx = mx-x;
	int dy = my-y;
	dx/=btnWidth;
	dy/=btnHeight;
	int pos =(dy*(w/btnWidth)+dx);
	return false;
}

void ActionBar::openAt(Game* g,int newx, int newy){
		Viewport *vp = g->getGUI()->getViewport();
		effectiveX=(vp->getViewportX()+newx)*BOX2D_Scale_Factor;
		effectiveY=(vp->getViewportY()+newy)*BOX2D_Scale_Factor;
		newx-=10;
		newy-=10;
		opened=true;
		display=true;
		int vpoffx=vp->getViewportOffsetX();
		int vpoffw=vp->getViewportWidth();
		int vpX=newx+w-vpoffx;
		int vpoffy=vp->getViewportOffsetY();
		int vpoffh=vp->getViewportHeight();
		int vpY=newy+h-vpoffy;
		if(vpX>vpoffw || vpY>vpoffh){
			if(vpX>vpoffw){
				x=vpX+(vpoffw-vpX)-w+vpoffx;
			}
			else{
				x=newx;
			}
			if(vpY>vpoffh){
				y=vpY+(vpoffh-vpY)-h+vpoffy;
			}
			else{
				y=newy;
			}
		}else{
			x=newx;
			y=newy;
		}
	}