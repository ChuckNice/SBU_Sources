#pragma once
#include <SourceCode\InGameCode\Unit.h>
#include <Box2D\Box2D.h>
#include "SSSF_SourceCode\gsm\physics\DynamicBody.h"
#include <SSSF_SourceCode\graphics\GameGraphics.h>
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\gsm\state\GameStateManager.h"
#include "SSSF_SourceCode\gsm\physics\Physics.h"
#include "SSSF_SourceCode\gsm\ai\BotRecycler.h"
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SourceCode\InGameCode\SensorListener.h>
#include "SSSF_SourceCode\gsm\physics\ActorHook.h"
#include <SourceCode\InGameCode\SensorFunctor.h>
#include <SourceCode\InGameCode\New folder\UnitHPBar.h>
#include <SourceCode\InGameCode\GroupGameSettings.h>
#include <SourceCode\InGameCode\Units\AttackCallback.h>

class Game;
class InGameState;
class Warrior: public Unit{
public:
	static float maxSpeedBase,attackSpeedBase,maxHealthBase,armorBase,frictionBase,attackRangeBase,sizeBase,aggroRange,unitMass,strengthBase;
	static wstring typeID;

	static float hpScaleX,hpScaleY;
	static float hpOffX,hpOffY;

	//Temporary "target" tower and target unit
	float attackRange;
	//Check if target is in range
	//If 0 no target
	//if 1 checking for target
	//if 2 target is in range
	int targetInRange;
	Unit* targetTower;
	ActorHook* hook;
	float moveForce;
	b2Fixture * fix;
	b2AABB closeArea;
	AttackCallback callback;
	//b2Fixture* clumpingFixture;
	Warrior();
	~Warrior(){
		delete hook;
	}

	void attackTargetedUnit(){
		state=attacking;
		//Setup animstate
	}
	void untargetUnit(){
		state=standby;
		targetUnit=NULL;
	}
	void isWithinRange(Unit * inRange);
	void leavesRange(Unit * inRange);
	void isWithinAttackRange(Unit * inRange);
	void leavesAttackRange(Unit* inRange);
	
	void handleOrders(Game* game);
	void determineWayPoint(InGameState* state,Game* game,float x, float y);
	inline float getMaxHealth(){return maxHealthBase*maxHealthFactor+maxHealthIncrease;}
	bool think(Game *game);
	Bot* clone(){
		Warrior* w = new Warrior();
		w->setCurrentState(getCurrentState());
		w->setSpriteType(getSpriteType());
		return	w;
	}
	
	void onAttack(Game* game, Unit* u, float damage);
	b2Fixture* getFix(){
		return fix;
	}
	const b2AABB& getAABB(){
		return fix->GetAABB(0);
	}
	static void loadSelf(lua_State* s);
	void recycle(Game* game,BotRecycler * recycle);
	void birth(int x, int y,Game* game){
		hp=getMaxHealth();
		armor= armorBase;
		attackSpeed=attackSpeedBase;
		strength=strengthBase;
		this->generalBirth();
		//create body and set up position
		AnimatedSpriteType *type = getSpriteType();
		targetArea.lowerBound.x=0;
		targetArea.lowerBound.y=0;
		targetArea.upperBound.x=aggroRange;
		targetArea.upperBound.y=aggroRange;
		closeArea.lowerBound.x=0;
		closeArea.lowerBound.y=0;
		closeArea.upperBound.x=attackRangeBase;
		closeArea.upperBound.y=attackRangeBase;
		b2BodyDef body;
		hook->type=UNIT_TYPE;
		hook->data=this;
		body.userData=hook;
		body.fixedRotation=true;
		body.position.x=(float)x*BOX2D_Scale_Factor;
		body.position.y=(float)y*BOX2D_Scale_Factor;
		registerBody(game,&body);
		//moveForce=game->getGSM()->getPhysics()->getTimeStep();
		//Small Circle collidable
		b2FixtureDef smallCollidable;
		b2CircleShape smallerCircle;
		smallerCircle.m_radius=this->sizeBase *BOX2D_Scale_Factor;
		smallCollidable.shape=&smallerCircle;
		fix=getBody()->CreateFixture(&smallCollidable);
		b2MassData data;
		this->getBody()->GetMassData(&data);
		data.mass=this->unitMass;
		this->getBody()->SetMassData(&data);
		this->velocityDampener=this->frictionBase; //Friction Force lower for light units ~.1 
		this->setMaxSpeed(this->maxSpeedBase);	//set max speed
		this->issueStopOrder();
		waypoints.clear();
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
	inline bool displayHPBar(){
		return false;
	}
	static void registerLua(lua_State *state){
		using namespace luabind;
		module(state)[
			class_<Warrior>("Warrior")
				.def("setCurrentState",&Warrior::setCurrentStateA)
				.def("setSpriteType",&Warrior::setSpriteType)
		];
	}
};
