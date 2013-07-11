#pragma once
#include <stdafx.h>
#include <SourceCode\InGameCode\Unit.h>
#include <SSSF_SourceCode\gsm\physics\ActorHook.h>
#include <SSSF_SourceCode\gsm\sprite\AnimatedSpriteType.h>
#include <SSSF_SourceCode\gsm\sprite\AnimatedSprite.h>
#include "SSSF_SourceCode\game\Game.h"
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <SSSF_SourceCode\graphics\GameGraphics.h>
#include <SSSF_SourceCode\gsm\ai\BotRecycler.h>
#include <SourceCode\InGameCode\New folder\UnitHPBar.h>

class Building:public Unit{
public:
	b2Fixture* fix;
	ActorHook* hook;
	float constructionPercent;
	int workers;
	int w,h;
	static int barFill,barOutline;
	Building(){
		hp=getMaxHealth();
		setVisible(true);
		setAlpha(255);
		hook=new ActorHook();
		setCurrentState(wstring(L"IDLE_STATE"));
		selected=false;
		w=h=0;
	}
	~Building(){
		delete hook;
	}
	virtual float getMaxHealth(){return 400;}
	virtual Bot* clone()=0;
	virtual Bot* clone(Game* g)=0;
	virtual float getBaseRange(){return 5;}
	virtual void mouseClick(Game* g, int x, int y)=0;
	virtual bool canBuild(Game* g, InGameState *state, Player* p)=0;
	void attackUnit(Game* g, float dmg, Unit*u);
	bool think(Game *game){
		if(isDead())return true;
		return false;
	}
	virtual int getInfluenceRange(){return constructionPercent>=100?4:0;}
	void addPreview(int mx, int my, RenderList* list){
		list->addRenderItem(this->getCurrentImageID(),mx-this->getSpriteType()->getTextureWidth()/2,my-this->getSpriteType()->getTextureHeight()/2,0,155,0,0,0);
		
	}
	void registerBody(Game* game, b2BodyDef *def){
		body = game->getGSM()->getPhysics()->createBody(def);
	}
	
	/*{
		Building *b = new Building();
		b->setSpriteType(getSpriteType());
		b->setCurrentState(wstring(L"IDLE_STATE"));
		b->w=w= (int)(std::ceil((float)(getSpriteType()->getPhysicalW()/TILE_WIDTH)));
		b->h=h= (int)(std::ceil((float)(getSpriteType()->getPhysicalH()/TILE_HEIGHT)));
		return b;
	}*/
	b2Fixture* getFix(){
		return fix;
	}
	const b2AABB& getAABB(){
		return fix->GetAABB(0);
	}
	void recycle(Game* game,BotRecycler * recycle){
		game->getGSM()->getPhysics()->destroyBody(this);
		recycle->recycleBot(L"BUILDING" ,this);
	}
	virtual void birth(int x, int y,Game* game){
		hp=getMaxHealth(); 
		constructionPercent=0;
		workers=0;
		//create body and set up position
		AnimatedSpriteType *type = getSpriteType();
		w= (int)(std::ceil((float)type->getPhysicalW()/TILE_WIDTH));
		h= (int)(std::ceil((float)type->getPhysicalH()/TILE_HEIGHT));
		b2BodyDef body;
		hook->type=BUILDING_TYPE;
		hook->data=this;
		body.userData=hook;
		body.fixedRotation=true;
		body.position.x=(float)x*BOX2D_Scale_Factor;
		body.position.y=(float)y*BOX2D_Scale_Factor;
		registerBody(game,&body);
		b2FixtureDef fix;
		fix.density=0;
		fix.restitution=1;
		fix.friction=0;
		b2PolygonShape shape;
		shape.SetAsBox(type->getPhysicalW()/2.0f*BOX2D_Scale_Factor,type->getPhysicalH()/2.0f*BOX2D_Scale_Factor);
		fix.shape=&shape;
		this->fix=getBody()->CreateFixture(&fix);

		UnitHPBar* bar=(UnitHPBar*)(game->getGSM()->getFXSys()->getParticle(UNITHPBARKEY));
		bar->u=this;
		bar->sX=this->getTextureWidth()/game->getGraphics()->getWorldTextureManager()->getTextureX(bar->containerID);
		bar->sY=bar->sX;
		bar->calcOffsets(this);
		game->getGSM()->getWorld()->addParticle(bar);
	}

};