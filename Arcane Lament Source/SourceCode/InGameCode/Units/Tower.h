#pragma once
#include <SourceCode\InGameCode\Unit.h>
#include "SSSF_SourceCode\gsm\physics\DynamicBody.h"
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\gsm\state\GameStateManager.h"
#include "SSSF_SourceCode\gsm\physics\Physics.h"
#include "SSSF_SourceCode\gsm\physics\ActorHook.h"
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SourceCode\InGameCode\New folder\UnitHPBar.h>
#include <SourceCode\InGameCode\GroupGameSettings.h>
class Game;
class BotRecycler;
class Tower:public Unit{
public:
	static float maxHealthBase, influenceRangeBase,armorBase;

	static float hpScaleX,hpScaleY;
	static float hpOffX,hpOffY;

	b2Fixture *fix;
	ActorHook* hook;
	Tower(){
		hp=getMaxHealth();
		setVisible(true);
		setAlpha(255);
		hook=new ActorHook();
		selected=false;
		
	}
	~Tower(){
		delete hook;
	}
	const b2AABB& getAABB(){
		return fix->GetAABB(0);
	}
	b2Fixture* getFix(){
		return fix;
	}
	static void loadSelf(lua_State* s);
	virtual float getMaxHealth(){return maxHealthBase;}
	bool think(Game *game){
		if(alertCooldown>0)alertCooldown--;
		if(isDead()){
			//(InGameState*)game->getGSM()->getManager();
			return true;
		}
		return false;
	}
	int getInfluenceRange(){return influenceRangeBase;}
	void registerBody(Game* game, b2BodyDef *def){
		body = game->getGSM()->getPhysics()->createBody(def);
	}
	Bot* clone(){

		return NULL;
	}
	void recycle(Game* game,BotRecycler * recycle){
		game->getGSM()->getPhysics()->destroyBody(this);
	}
	void birth(int x, int y,Game* game){
		hp=getMaxHealth();
		armor=armorBase;
		//create body and set up position
		AnimatedSpriteType *type = getSpriteType();
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
	}
	void attackUnit(Game* g, float dmg,Unit* u);
	void mouseClick(Game* g, int x, int y);
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
	static void registerLua(lua_State *state){
		using namespace luabind;
		module(state)[
			class_<Tower>("Tower")
				.def("setCurrentState",&Tower::setCurrentStateA)
				.def("recycle",&Tower::markForRecycle)
				.def("setSpriteType",&Tower::setSpriteType)
		];
	}
};