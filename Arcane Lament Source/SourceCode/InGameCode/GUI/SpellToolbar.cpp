#include <SourceCode\InGameCode\GUI\SpellToolbar.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\graphics\RenderList.h>
#include <stdafx.h>
#include <SourceCode\InGameCode\GUI\SpellButton.h>
#include <SourceCode\InGameCode\Spells\Spell.h>
#include <SourceCode\InGameCode\SpellDB.h>
#include <SSSF_SourceCode\sound\SoundManager.h>
#include "SSSF_SourceCode\text\GameText.h"
#include <SourceCode\InGameCode\InGameState.h>
#include <SSSF_SourceCode\gsm\state\GameManager.h>
#include <SourceCode\InGameCode\Player.h>
#include <SourceCode\WizardSystem\Wizard.h>

//#include "luabind\luabind.hpp"
//#include "lua.h"

void SpellToolbar::initializeToolbar(Game* game ){
	clearToolbar();
	InGameState* iGs = (InGameState*)game->getGSM()->getManager();
	wizard = iGs->getPlayer()->getWizard();

	//GET THE WIZARDS TOOLBAR ORDER
	multimap<int, string> spellOrder = wizard->getToolbarOrder();
	SpellDB* sDB = iGs->getSpellDB();
	int toolbarPos = 0;

	for(multimap<int, string>::iterator it = spellOrder.begin(); it != spellOrder.end() && toolbarPos<wizard->getMaxToolbars()*wizard->getAvailSlots(); toolbarPos++ ){
		if ((*it).first == toolbarPos){
			string spellName = (*it).second;
			this->addSpell(sDB->getSpellButton(spellName), sDB->getSpell(spellName), (*it).first);
			it++;
		}else
			this->addSpell(sDB->getSpellButton("ButtonSocket"), NULL, (*it).first);
	}
	while(toolbarPos++<wizard->getMaxToolbars()*wizard->getAvailSlots())
				this->addSpell(sDB->getSpellButton("ButtonSocket"), NULL, 0);
}

// ADD SPELL
void SpellToolbar::addSpell(SpellButton *btn, Spell *p, int pos){

	btns.push_back(btn);
	spells.push_back(p);

}

// REMOVE SPELL
void  SpellToolbar::removeSpell(SpellButton *btn, Spell *p){
	btns.push_back(btn);
	spells.push_back(p);
}


void SpellToolbar::snapToToolbar(int *mx,int *my){
	if(*mx<x||*mx>x+w||*my<y||*my>y+h)return;
	int dx = *mx-x;
	int dy = *my-y;
	dx/=btnWidth;
	dy/=btnHeight;
	unsigned int pos =(dy*(w/btnWidth)+dx);
	if(pos<btns.size()) {
		*mx=x+dx*btnWidth;
		*my=y+dy*btnHeight;
	}
}

int	SpellToolbar::getPosAtMouse(int mx, int my){
	if(mx<x||mx>x+w||my<y||my>y+h)return -1;
	int dx = mx-x;
	int dy = my-y;
	dx/=btnWidth;
	dy/=btnHeight;
	unsigned int pos =(dy*(w/btnWidth)+dx);
	if(pos<btns.size()) {
		return pos;
	}
	return -1;
}

void SpellToolbar::handleClick(Game* g,int mx, int my){
	if(mx<x||mx>x+w||my<y||my>y+h)return;
	int dx = mx-x;
	int dy = my-y;
	dx/=btnWidth;
	dy/=btnHeight;
	unsigned int pos =(dy*(w/btnWidth)+dx);
	if(pos<btns.size()) {
		if(btns.at(pos)->title != "ButtonSocket"){
			currentDown=pos;
			//g->getSoundManager()->playCue("VICTORY");
		}
	}
}

void SpellToolbar::handleMouse(Game* g,int mx, int my){

	// MOUSE OVER TOOLTIPS 
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
	int pos =(dy*(w/btnWidth)+dx);
	if(pos<btns.size()){
		if(hotPos!=pos){
			hotPos=pos;
			hotTimer=0;
			if(textBox!=NULL){
				textBox->release();
				textBox=NULL;
			}
			SpellButton *b = btns.at(pos);
			textBox=g->createTextBoxSimple(b->title,"",mx,my-40,200);
			g->setTitle(textBox,b->title,b->titleFont,18,b->titleColor[0],b->titleColor[1],b->titleColor[2],b->titleColor[3]);
			g->setBG(textBox,b->backgroundID,10,5);
		}
		else hotTimer++;
		if(hotTimer==24){
			textBox->release();
			SpellButton *b = btns.at(pos);
			textBox=g->createTextBoxSimple(b->title,b->tooltip,textBox->x,textBox->y-60,200);
			g->setTitle(textBox,b->title,b->titleFont,18,b->titleColor[0],b->titleColor[1],b->titleColor[2],b->titleColor[3]);
			g->setBody(textBox,b->tooltip,b->tooltipFont,16,b->tooltipColor[0],b->tooltipColor[1],b->tooltipColor[2],b->tooltipColor[3]);
			g->setBG(textBox,b->backgroundID,10,5);
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

void SpellToolbar::render(RenderList* list){
	int wBtns = w/btnWidth;
	for(int i=0;i<btns.size();i++){
		SpellButton *b= btns.at(i);
		if(i!=currentDown)
			list->addRenderItem(b->upImg,
								x+(i%wBtns)*btnWidth,
								y+i/wBtns*btnHeight,
								0,
								255,
								btnWidth,
								btnHeight
								);
		else 
			list->addRenderItem(b->downImg,
								x+(i%wBtns)*btnWidth,
								y+i/wBtns*btnHeight,
								0,
								255,
								btnWidth,
								btnHeight
								);
	}
}