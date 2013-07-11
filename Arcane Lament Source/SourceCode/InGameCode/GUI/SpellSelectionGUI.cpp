#include <stdafx.h>
#include <SourceCode\InGameCode\GUI\SpellSelectionGUI.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SourceCode\ProjectBase\ResourceBank.h>
#include <SSSF_SourceCode\graphics\RenderList.h>
#include <SourceCode\InGameCode\GUI\SpellDBToolbar.h>
#include <SourceCode\InGameCode\GUI\SpellToolbar.h>
#include <SourceCode\InGameCode\InGameState.h>
#include <SourceCode\InGameCode\SpellDB.h>

#define spdIT multimap<int, string>::iterator

SpellSelectionGUI::SpellSelectionGUI(void):spDBToolbar(610,200,250,400,50,50), spellToolbar(130,200,200,150,50,50){
	
}


SpellSelectionGUI::~SpellSelectionGUI(void){
	//delete spellToolbar;
}

void SpellSelectionGUI::setupScreen( Game *g, lua_State *state, SpellDB *spellDB){
	
	game = g;
	iGs = (InGameState*)g->getGSM()->getManager();
	player = iGs->getPlayer();
	popupOverlayID = ResourceBankgetResource((string)"InGameSpellSelectScreen");
	spDBToolbar.initializeToolbar(iGs, spellDB);
	spellToolbar.initializeToolbar(g);
	spellToolbar.resetCurrentDown();

}


void SpellSelectionGUI::handleMouse(int x, int y){
	this->x=x;
	this->y=y;
	spellToolbar.snapToToolbar(&this->x,&this->y);
	spDBToolbar.handleMouse(iGs, game, x, y);
	map<string,pair<SpellButton*, Spell*>>::iterator search= iGs->getSpellDB()->getSpellDB()->find(spDBToolbar.currentSpell);
	if(search==iGs->getSpellDB()->getSpellDB()->end())
		spellToolbar.handleMouse(game, x, y);
}
void SpellSelectionGUI::handleClick(int x, int y){
	map<string,pair<SpellButton*, Spell*>>::iterator search= iGs->getSpellDB()->getSpellDB()->find(spDBToolbar.currentSpell);
	if(search!=iGs->getSpellDB()->getSpellDB()->end()){
		int atPos= spellToolbar.getPosAtMouse(x,y);
		if(atPos!=-1){
			spdIT search = iGs->mainPlayer->wizard->getToolbarOrder().find(atPos);
			if(search!=iGs->mainPlayer->wizard->getToolbarOrder().end()){
				string replaced=search->second;
				iGs->mainPlayer->wizard->getToolbarOrder().erase(search);
				iGs->mainPlayer->wizard->getToolbarOrder().insert(make_pair(atPos,spDBToolbar.currentSpell));
				spDBToolbar.currentSpell=replaced;
			}else{
				iGs->mainPlayer->wizard->getToolbarOrder().insert(make_pair(atPos,spDBToolbar.currentSpell));
				spDBToolbar.currentSpell="";
			}
			spellToolbar.initializeToolbar(game);
			spDBToolbar.initializeToolbar(iGs,iGs->getSpellDB());
			iGs->toolbar.initializeToolbar(game);
		}
		else{
			spDBToolbar.currentSpell="";
		}
	}else{
		int atPos= spellToolbar.getPosAtMouse(x,y);
		if(atPos!=-1){
			spdIT search = iGs->mainPlayer->wizard->getToolbarOrder().find(atPos);
			if(search!=iGs->mainPlayer->wizard->getToolbarOrder().end()){
				spDBToolbar.currentSpell=search->second;
				iGs->mainPlayer->wizard->getToolbarOrder().erase(search);
			}
			
			spellToolbar.initializeToolbar(game);
			spDBToolbar.initializeToolbar(iGs,iGs->getSpellDB());
			iGs->toolbar.initializeToolbar(game);
		}
	}
	spDBToolbar.handleClick(iGs, game, x, y);
	//spellToolbar.handleClick(game, x, y);
}
void SpellSelectionGUI::handleClickDrag(int xi, int yi,int xf,int yf){

}
void SpellSelectionGUI::dispatchClickDrag(int xi, int yi,int xf,int yf){

}
void SpellSelectionGUI::render(RenderList* list){
	list->addRenderItem(popupOverlayID, 0, 0, 0, 255, 0, 0 );
	spDBToolbar.render(list);
	spellToolbar.render(list);
	map<string,pair<SpellButton*, Spell*>>::iterator search= iGs->getSpellDB()->getSpellDB()->find(spDBToolbar.currentSpell);
	if(search!=iGs->getSpellDB()->getSpellDB()->end()){
		list->addRenderItem(search->second.first->upImg,
			x,y,0,255,0,0);
	}
}


void SpellSelectionGUI::update(Game* g){

}