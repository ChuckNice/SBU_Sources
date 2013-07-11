#include "SourceCode\InGameCode\StatusFX\SelectedFX.h"
#include "SourceCode\InGameCode\StatusFX.h"
#include <SSSF_SourceCode\graphics\RenderList.h>
#include <SourceCode\InGameCode\Unit.h>
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <SourceCode\InGameCode\InGameState.h>
StatusFactory<SelectedFX> SelectedFX::factory;
int SelectedFX::displayID=0;
int SelectedFX::textureW=0;
int SelectedFX::textureH=0;
void SelectedFX::render(RenderList* list, int x,int y){
	list->addRenderItem(displayID,x-textureW/2.0f,y-textureH*.3f,0,255,0,0,0,sX,sY);
}
void SelectedFX::applyToUnit(Unit* u){
	me=u;
	sX=u->getTextureWidth()/textureW;
	sY=u->getTextureHeight()/textureH;
}
void SelectedFX::release(){
	factory.recycle(this);
}

void SelectedFX::birth(Game* g){
}