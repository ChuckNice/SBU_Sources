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
#include <SourceCode\InGameCode\Building.h>

class RenderList;
class ScoutTower:public Building{
public:
	static float goldCost,influenceRange,constructionPercentBase,constructionPercentIncrement;
	static float maxHealthBase, armorBase;
	static wstring typeID;
	static float hpScaleX,hpScaleY;
	static float hpOffX,hpOffY;
	int currentPercent;
	ScoutTower(){
		hp=getMaxHealth();
		setVisible(true);
		setAlpha(255);
		setCurrentState(wstring(L"IDLE_STATE"));
		selected=false;	
		currentPercent=0;
	}
	~ScoutTower(){
		delete hook;
	}
	static void loadSelf(lua_State* s);
	virtual float getMaxHealth(){return maxHealthBase;}
	virtual Bot* clone();
	virtual void render(RenderList* list,Viewport* vp);
	virtual void mouseClick(Game* g, int x, int y);
	virtual bool canBuild(Game* g, InGameState *state, Player* p);	
	
	virtual Bot* clone(Game* g){
		return g->getGSM()->getSpriteManager()->getBotRecycler()->retrieveBot(typeID);
	}
	bool think(Game *game);

	void addPreview(int mx, int my, RenderList* list){
		list->addRenderItem(this->getCurrentImageID(),mx,my,0,155,0,0,0);
		
	}
	inline int getInfluenceRange(){return constructionPercent>=100?influenceRange:0;}
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
		recycle->recycleBot(L"ScoutTower" ,this);
		this->generalRelease();
	}
	void birth(int x, int y,Game* game){
		hp=getMaxHealth();
		armor=armorBase;
		currentPercent=0;
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

		p->gold-=goldCost;
	}
	inline float getHPOffX(){
		return hpOffX;
	}
	inline float getHPOffY(){
		return hpOffY;
	}
	inline float getHPScaleX(){
		return hpScaleX;
	}
	inline float getHPScaleY(){
		return hpScaleY;
	}
};