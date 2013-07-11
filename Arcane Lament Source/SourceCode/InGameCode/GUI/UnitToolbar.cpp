#include <SourceCode\InGameCode\GUI\UnitToolbar.h>
#include <SourceCode\ProjectBase\ResourceBank.h>
#include <SourceCode\InGameCode\Unit.h>
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

void UnitToolbar::initializeToolbar( InGameState* s){
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
		p=s->mainPlayer;
	}


void UnitToolbar::handleMouse(InGameState* state,Game* g,int mx, int my){
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
	string test= "QWERTASDFZXCV";
	if(pos<Player::unitTypes.size()){
		if(hotPos!=pos){
			hotPos=pos;
			hotTimer=0;
			if(textBox!=NULL){
				textBox->release();
				textBox=NULL;
			}
			UnitEntry b = Player::unitTypes.at(pos);
			stringstream stream;
			stream<<"Cost:"<<b.goldCost;
			stream<<"\nIncome Increase:"<<b.incomeIncrease[0];
			textBox=g->createTextBoxSimple("",stream.str(),mx,my-40,200);
			g->setBody(textBox,stream.str(),convertWStr(b.tooltipFont),16,b.tooltipColor[0],b.tooltipColor[1],b.tooltipColor[2],b.tooltipColor[3]);
			stream.str("");
			stream<<convertWStr(b.title);
			stream<<"("<<test.at(pos)<<")";
			g->setTitle(textBox,stream.str(),convertWStr(b.titleFont),18,b.titleColor[0],b.titleColor[1],b.titleColor[2],b.titleColor[3]);
			
			g->setBG(textBox,bgID,10,5);
		}
		else hotTimer++;
		if(hotTimer==24){
			textBox->release();
			UnitEntry b = Player::unitTypes.at(pos);
			stringstream stream;
			stream<<"Cost:"<<b.goldCost;
			stream<<"\nIncome Increase:"<<b.incomeIncrease[0];
			textBox=g->createTextBoxSimple("","",mx,my-40,200);
			stream<<"\n"<<convertWStr(b.tooltip);
			g->setBody(textBox,stream.str(),convertWStr(b.tooltipFont),16,b.tooltipColor[0],b.tooltipColor[1],b.tooltipColor[2],b.tooltipColor[3]);
			stream.str("");
			stream<<convertWStr(b.title);
			stream<<"("<<test.at(pos)<<")";
			g->setTitle(textBox,stream.str(),convertWStr(b.titleFont),18,b.titleColor[0],b.titleColor[1],b.titleColor[2],b.titleColor[3]);
			g->setBG(textBox,bgID,10,5);
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
void UnitToolbar::render(RenderList* list){
	int wBtns = w/btnWidth;
	for(int i=0;i<Player::unitTypes.size();i++){
		UnitEntry b= Player::unitTypes.at(i);
		list->addRenderItem(b.imageID,
			x+(i%wBtns)*btnWidth,
			y+i/wBtns*btnHeight,
			0,
			255,
			btnWidth,
			btnHeight
			);
	}
	int numBuild;
	int type=-1;
	int numDisplayed=0;
	if(!p->buildStack.empty())for(int i=0;i<p->buildStack.size();i++){
		if(type!=p->buildStack[i].second){
			if(type!=-1){
				UnitEntry b= Player::unitTypes.at(type);
				list->addRenderItem(b.imageID,
					0,
					sideY+numDisplayed*30,
					0,
					255,
					btnWidth,
					btnHeight
					);
				wstringstream myInt;
				myInt<<numBuild;
				displayStrs[numDisplayed]=(myInt.str());
				list->addRenderItem(10,10+sideY+numDisplayed*30,255,255,255,255,&displayStrs[numDisplayed],&font,15);
				numDisplayed++;
				if(numDisplayed==10){
					type=-1;
					break;
				}
			}
			numBuild=0;
			type=p->buildStack[i].second;
		}
		numBuild++;
	}
	if(type!=-1){
				UnitEntry b= Player::unitTypes.at(type);
				list->addRenderItem(b.imageID,
					0,
					sideY+numDisplayed*30,
					0,
					255,
					btnWidth,
					btnHeight
					);
				wstringstream myInt;
				myInt<<numBuild;
				displayStrs[numDisplayed]=(myInt.str());
				list->addRenderItem(10,10+sideY+numDisplayed*30,255,255,255,255,&displayStrs[numDisplayed],&font,15);
			}
}
void UnitToolbar::handleClick(InGameState* state,Game* g, int mx, int my){
	if(mx<x||mx>x+w||my<y||my>y+h)return;

}