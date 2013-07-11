#pragma once
#include "stdafx.h"
#include <SSSF_SourceCode\gsm\ai\Bot.h>
#include "luabind\luabind.hpp"
//#include "lua.h"
#include <SSSF_SourceCode\stdutil.h>
#include "SSSF_SourceCode\gsm\sprite\AnimatedSpriteType.h"
#include "SSSF_SourceCode\graphics\RenderList.h"
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SourceCode\InGameCode\Orders.h>
class Player;
class StatusFX;
struct b2AABB;
enum UnitState{
	standby,attacking
};
class Unit:public Bot{
protected:
	float maxSpeed;
	float velocityDampener;
	float movementImpulse;
	UnitState state;
	float armor,attackSpeed,strength;
	float maxSpeedFactor,attackSpeedFactor,maxHealthFactor,armorFactor,strengthFactor;
	float maxHealthIncrease,armorIncrease,strengthIncrease;
public:
	static bool cyclicSelect;
	int  birthID;
	Unit* targetUnit;
	int birthIDTarget;
	Orders order;
	bool selected;
	float selectedScaleX,selectedScaleY;
	list<StatusFX*> statusFXList;
	float hp;
	Player * p;
	int isDisturbed;
	int attackCooldown;
	int alertCooldown;
	b2AABB targetArea;
	vector<pair<float,float>> waypoints;
	int lastWaypointGeneration;
	virtual b2Fixture* getFix()=0;
	virtual const b2AABB& getAABB()=0;
	//Should be used for AI setup however you like. last Think
	int lastThink;
	Unit(){
		attackCooldown=alertCooldown=0;
		lastThink=0;
		velocityDampener=.2f;
		maxSpeedFactor=attackSpeedFactor=maxHealthFactor=armorFactor=strengthFactor=1.0f;
		maxHealthIncrease=armorIncrease=strengthIncrease=0;
		order.type=STOPORDER;
		p=NULL;
	}
	void setTargetUnit(Unit* target){
		targetUnit=target;
		birthIDTarget=target->birthID;
	}
	float getMovementImpulse(){
		return movementImpulse;
	}
	float getVelocityDampener(){
		return velocityDampener;
	}
	void setMaxSpeed(float speed){
		maxSpeed=speed;
		movementImpulse=maxSpeed*velocityDampener*getBody()->GetMass();
	}
	virtual bool displayHPBar(){
		return true;
	}
	void issueStopOrder(){
		order.type=STOPORDER;
		order.initialX=this->getWorldX();
		order.initialY=this->getWorldY();
		this->waypoints.clear();
		lastThink=lastWaypointGeneration=0;
		targetUnit=NULL;
	}
	void issueAttackToOrder(int x, int y){
		order.type=ATTACKTOORDER;
		order.x=(float)x;
		order.y=(float)y;
		this->waypoints.clear();
		lastThink=lastWaypointGeneration=0;
		targetUnit=NULL;
	}
	void issuePatrolToOrder(int x, int y){
		order.type=PATROLTOORDER;
		order.x=(float)x;
		order.y=(float)y;
		order.initialX=this->getWorldX();
		order.initialY=this->getWorldY();
		this->waypoints.clear();
		lastThink=lastWaypointGeneration=0;
		targetUnit=NULL;
	}
	void issueMoveOrder(int x,int y){
		order.type=MOVEORDER;
		order.x=(float)x;
		order.y=(float)y;
		this->waypoints.clear();
		lastThink=lastWaypointGeneration=0;
		targetUnit=NULL;
	}
	void issueAttackUnitOrder(Unit* specificUnit){
		order.type=ATTACKUNITORDER;
		targetUnit=specificUnit;
		birthIDTarget=specificUnit->birthID;
		this->waypoints.clear();
		lastThink=lastWaypointGeneration=0;
	}
	inline float getWorldX(){
		if(body==NULL)return -1.0f;
		else return (body->GetPosition().x);
	}
	inline float getWorldY(){
		if(body==NULL)return -1.0f;
		else return (body->GetPosition().y);
	}
	inline int getX(){
		if(body==NULL)return -1;
		else return (int)(body->GetPosition().x*BOX2D_Scale_Factor_INV);
	}
	inline int getY(){
		if(body==NULL)return -1;
		else return (int)(body->GetPosition().y*BOX2D_Scale_Factor_INV);
	}
	void damageUnit(float damage){
		hp-=max(damage-armor,1.0f);
	}
	void addStatusFX(StatusFX* fx);
	bool isDead(){
		return hp<=0;
	}
	float getTextureHeight(){
		return (float)getSpriteType()->getTextureHeight();
	}
	float getTextureWidth(){
		return  (float)getSpriteType()->getTextureWidth();
	}
	void setCurrentStateA(string state){
		setCurrentState(convertwstrX(state));
	}
	void generalBirth();
	void generalRelease();
	void disturb(int x){isDisturbed=x;}
	void disturb(){isDisturbed=5;}
	virtual float getHPOffX()=0;
	virtual float getHPOffY()=0;
	virtual float getHPScaleX()=0;
	virtual float getHPScaleY()=0;
	virtual void render(RenderList *list,Viewport *vp);
	inline float strengthEstimate(){return strength;}
	virtual void birth(int x,int y,Game* game){}
	float getHealth(){return hp;}
	virtual float getMaxHealth()=0;
	static void registerLua(lua_State *state){
		using namespace luabind;
		module(state)[
			class_<Unit>("Unit")
				.def("setCurrentState",&Unit::setCurrentStateA)
				.def("recycle",&Unit::markForRecycle)
				.def("setSpriteType",&Unit::setSpriteType)
		];
	}
	void renderStatusFX(RenderList* list, int xOff,int yOff);
	virtual void attackUnit(Game* g, float damage, Unit* source);
	
	void checkAlerts(Game* game);
	void targetThisUnit(Unit* u){
		targetUnit=u;
		birthIDTarget=u->birthID;
	}
};