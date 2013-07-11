#pragma once
#include "stdafx.h"
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\graphics\GameGraphics.h"
#include "SSSF_SourceCode\graphics\RenderList.h"
#include "SSSF_SourceCode\gui\Button.h"
#include "SSSF_SourceCode\gui\OverlayImage.h"
#include "SSSF_SourceCode\gsm\sprite\AnimatedSprite.h"
#include "SSSF_SourceCode\gui\Cursor.h"
#include "SSSF_SourceCode\gui\ScreenGUI.h"
#include <SourceCode\InGameCode\Player.h>
#include <SourceCode\InGameCode\Units\Tower.h>
#include "SSSF_SourceCode\graphics\OverlayRenderer.h"
#include <SourceCode\InGameCode\Unit.h>

class InGameRenderer:public OverlayRenderer
{
//TopBarHealthContainer
//TopBarHealthFill
//TopBarManaContainer
//TopBarManaFill
//MiniMap
	int healthBarContainer,healthBarFill,manaBarContainer,manaBarFill,minimap;

public:
	Player* p;
	InGameRenderer(){
		p=NULL;
	}
	void setHBC(int id){healthBarContainer=id;}
	void setHBF(int id){healthBarFill=id;}
	void setMBC(int id){manaBarContainer=id;}
	void setMBF(int id){manaBarFill=id;}

	//OVERRIDDEN addRenderItemsToList
	void addRenderItemstoList(RenderList* list){
		//This renders onto the in game screen
		if(p==NULL)
			list->addRenderItem(healthBarFill,0,1,0,255,0,0,0,.5f,1.0f,1);
		else
			list->addRenderItem(healthBarFill,0,1,0,255,0,0,0,(float)p->tower->getHealth()/p->tower->getMaxHealth(),1.0f,1);
		list->addRenderItem(healthBarContainer,0,1,0,255,0,0);
		if(p==NULL)
			list->addRenderItem(manaBarFill,1,12,0,255,0,0,0,.5f,1.0f,1);
		else
			list->addRenderItem(manaBarFill,1,12,0,255,0,0,0,(float)p->mana/p->maxmana,1.0f,1);
		list->addRenderItem(manaBarContainer,1,12,0,255,0,0);
	}
};