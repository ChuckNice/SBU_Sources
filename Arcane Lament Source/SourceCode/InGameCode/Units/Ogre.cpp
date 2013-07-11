#include "SourceCode\InGameCode\Units\Ogre.h"
#include <SourceCode\InGameCode\Unit.h>
#include "SSSF_SourceCode\gsm\physics\DynamicBody.h"
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\gsm\state\GameStateManager.h"
#include "SSSF_SourceCode\gsm\physics\Physics.h"
#include "SSSF_SourceCode\gsm\ai\BotRecycler.h"
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SourceCode\InGameCode\SensorListener.h>
#include "SSSF_SourceCode\gsm\physics\ActorHook.h"
#include <SourceCode\InGameCode\SensorFunctor.h>
#include <SourceCode\InGameCode\AABBCallback.h>
#include <SourceCode\InGameCode\Units\AttackCallback.h>
#include <SourceCode\InGameCode\InGameState.h>
#include <SourceCode\InGameCode\FX\GenericFXParticles.h>

//INSTANTIATE TYPEID
wstring Ogre::typeID= L"OGRE";
float Ogre::maxSpeedBase=2;
float Ogre::attackSpeedBase=4;
float Ogre::maxHealthBase=50;
float Ogre::armorBase=50;
float Ogre::frictionBase=.05f;
float Ogre::attackRangeBase=50;
float Ogre::sizeBase=10;
float Ogre::aggroRange=10;
float Ogre::unitMass=10;
float Ogre::strengthBase=50;


float Ogre::hpScaleX,Ogre::hpScaleY;
float Ogre::hpOffX,Ogre::hpOffY;


//////////////////////// CLASS - CloseRange /////////////////////////////
/*
*	
*
*/
class CloseRange:public CallbackFunctor{
public:
	/*
	* Overloaded operator () -returns true if the 2nd param 
	*		is an ally.
	*/
	bool operator()(Unit* self, Unit* other){
		if(!self->p->isAlly(other)){
			((Ogre*)self)->setTargetUnit(other);
			return false;
		}else return true;
	}
};


//////////////////////// CLASS - Ogre /////////////////////////////
/*
*	Constructor - 
*
*/
Ogre::Ogre(){
	state=standby;
	hp=getMaxHealth();
	setVisible(true);
	setAlpha(255);
	targetUnit=targetTower=NULL;
	hook=new ActorHook();
	targetInRange=0;
	callback.myUnit=this;
}

/*
*	isWithinAttackRange - 
*
*/
void Ogre::isWithinAttackRange(Unit * inRange){
	if(!p->isAlly(inRange)){
		if(targetInRange==0){
			targetUnit=inRange;
			targetInRange=1;
			attackCooldown=attackSpeedBase*attackSpeedFactor; 
			//Set State to Attack
		}
	}
}

/*
*	leavesRange - 
*
*/
void Ogre::leavesRange(Unit * inRange){
}

/*
*	leavesAttackRange - 
*
*/
void Ogre::leavesAttackRange(Unit * inRange){
		if(inRange==targetUnit){
			targetInRange=0;
			//Cant attack, set state to walk
		}
	
}



void Ogre::loadSelf(lua_State* s){

}

/*
*	think - 
*
*/
bool Ogre::think(Game *game){
		if(alertCooldown>0)alertCooldown--;
		if(attackCooldown>0)attackCooldown--;
		if(isDead() && this->getCurrentState()==DEATH_STATE && justStartedState())
			return true;
		else if(isDead() && this->getCurrentState()!=DEATH_STATE){
			this->setCurrentState((wstring)DEATH_STATE);
			body->SetActive(false);
			return true;
		}
		else if (isDead())
			return false;
		
		if(getBody()->GetLinearVelocity().x>0){ if(getCurrentState()!=L"IDLE_STATE_SOUTH")this->setCurrentState((wstring)L"IDLE_STATE_SOUTH");}
		else if(getCurrentState()!=L"IDLE_STATE_NORTH")this->setCurrentState((wstring)L"IDLE_STATE_NORTH");
		if(isDisturbed<=0){
			b2Vec2 vel=getBody()->GetLinearVelocity();
			//vel.Normalize();
			vel*=-this->getVelocityDampener()*movementImpulse;
			getBody()->ApplyForceToCenter(vel);
		}else{
			if(order.type!=ATTACKUNITORDER)untargetUnit();
			if(isDisturbed>0)isDisturbed--;
			return false;
		}
		if(targetUnit!=NULL&&(targetUnit->isDead()||targetUnit->birthID!=birthIDTarget)){
			untargetUnit();
			if(order.type==ATTACKUNITORDER)
				order.type=STOPORDER;
		}
		if(targetUnit!=NULL&&state==attacking&& attackCooldown<=0){
			onAttack(game,targetUnit,strengthBase*strengthFactor+strengthIncrease);
			state=standby;
			attackCooldown=attackSpeedBase*attackSpeedFactor; 
		}
		if(lastThink--<=0){
			Physics* p =game->getGSM()->getPhysics();
			b2World* w=p->getWorld();
			if(order.type!=MOVEORDER)
				if(targetUnit==NULL){
					targetArea.upperBound-=targetArea.lowerBound;
					targetArea.lowerBound.x=getBody()->GetPosition().x-targetArea.upperBound.x/2.0f;
					targetArea.lowerBound.y=getBody()->GetPosition().y-targetArea.upperBound.y/2.0f;
					targetArea.upperBound+=targetArea.lowerBound;
					w->QueryAABB((&callback),targetArea);
				}
			 if(targetUnit!=NULL){
				closeArea.upperBound-=closeArea.lowerBound;
				closeArea.lowerBound.x=getBody()->GetPosition().x-closeArea.upperBound.x/2.0f;
				closeArea.lowerBound.y=getBody()->GetPosition().y-closeArea.upperBound.y/2.0f;
				closeArea.upperBound+=closeArea.lowerBound;
				if(b2TestOverlap(targetUnit->getAABB(),closeArea)){	
					attackTargetedUnit();
					b2Vec2 vel=getBody()->GetLinearVelocity();
					vel*=.3f;
					getBody()->SetLinearVelocity(vel);
				}
				else{
					state=standby;
					if(order.type!=ATTACKUNITORDER){
						AABBCallback<CloseRange> range;
						CloseRange c;
						range.f=&c;
						range.self=this;
						w->QueryAABB((&range),closeArea);
					}
				}
					
			}
			lastThink=10;
		}
		if(!waypoints.empty()){
			pair<float,float> coord=waypoints.back();
			float dx =(coord.first-getWorldX());
			float dy = (coord.second-getWorldY());
			b2Vec2 displacement(dx,dy);
			if(displacement.Length()<2){
				waypoints.pop_back();
			}
			displacement.Normalize();
			displacement*=this->getMovementImpulse();
			body->ApplyForceToCenter(displacement);
		}

		if(lastWaypointGeneration--<=0)
			handleOrders(game);
		return false;
	}

void Ogre::determineWayPoint(InGameState* state,Game* game,float x, float y){
	b2Vec2 pos = getBody()->GetPosition();
	pos.x-=x;
	pos.y-=y;
	if(pos.Length()>8){
		state->gridMap.aStarAlgorithm(game,getWorldX(),getWorldY(),x,y,&waypoints);	
		lastWaypointGeneration=60;
	}else{
		waypoints.clear();
		waypoints.push_back(make_pair(x,y));
		lastThink=lastWaypointGeneration=5;
	}
}

void Ogre::handleOrders(Game* game){
	InGameState *manager = (InGameState*)game->getGSM()->getManager();
	if(order.type==MOVEORDER){
		determineWayPoint(manager,game,order.x,order.y);
		float dx = order.x-getWorldX();
		float dy = order.y-getWorldY();
		b2Vec2 displacement(dx,dy);
		if(displacement.Length()<warriorFindTargetRange/2*BOX2D_Scale_Factor)
			this->issueStopOrder();

	}
	else if(targetUnit==NULL && order.type==PATROLTOORDER){
		determineWayPoint(manager,game,order.x,order.y);
		float dx = (float)(order.x-getWorldX());
		float dy = (float)(order.y-getWorldY());
		b2Vec2 displacement(dx,dy);
		if(displacement.Length()<warriorFindTargetRange/2*BOX2D_Scale_Factor){
			int tmp = order.x;
			order.x=order.initialX;
			order.initialX=tmp;
			tmp = order.y;
			order.y=order.initialY; 
			order.initialY=tmp;
		}
	}
	else if(order.type==ATTACKTOORDER && targetUnit==NULL){
		determineWayPoint(manager,game,order.x,order.y);
		float dx = order.x-getWorldX();
		float dy = order.y-getWorldY();
		b2Vec2 displacement(dx,dy);
		if(displacement.Length()<warriorFindTargetRange/2*BOX2D_Scale_Factor)
			this->issueStopOrder();

	}
	else if(order.type==ATTACKUNITORDER && targetUnit!=NULL)
		determineWayPoint(manager,game,targetUnit->getWorldX(),targetUnit->getWorldY());
	else if(targetUnit!=NULL && state==standby){
		determineWayPoint(manager,game,targetUnit->getWorldX(),targetUnit->getWorldY());
	}
	else lastWaypointGeneration=10;
}

/*
*	recycle - 
*
*/
void Ogre::recycle(Game* game,BotRecycler * recycle){
	game->getGSM()->getPhysics()->destroyBody(this);
	recycle->recycleBot(Ogre::typeID,this);
	targetUnit=NULL;
	targetInRange=0;
	setCurrentState((wstring)L"IDLE_STATE_NORTH");
	state=standby;
	this->generalRelease();
}

/*
*	isWithinRange - 
*
*/
void Ogre::isWithinRange(Unit* inRange){
	if(!p->isAlly(inRange)){ 
			if(targetUnit==NULL){
				targetUnit=inRange;
			}
		}
}

class SplashDamageCallback:public b2QueryCallback{
public:
	Game* g;
	Unit * me;
	b2Vec2 epicenter;
	float damage;
	float invrange;
	bool ReportFixture(b2Fixture* fixture)
	{
		if(fixture->IsSensor())return true;
		b2Body* body = fixture->GetBody();
		ActorHook* data = (ActorHook*)body->GetUserData();
		if(data->type==UNIT_TYPE){
			Unit* inRange = (Unit*)data->data;
			if(inRange!=me && !me->p->isAlly(inRange)){
				b2Vec2 diff = inRange->getBody()->GetPosition()-epicenter;
				float dmg = damage*diff.Length()*invrange;
				diff*=invrange*40;
				inRange->getBody()->ApplyForceToCenter((diff));
				inRange->attackUnit(g,dmg,me);
				if(inRange->isDead()){
					me->p->numKills++;
				}
			}
		}
		return true;
	}
};

void Ogre::onAttack(Game* game, Unit* u, float damage){
	FXParticleSprite *arrow=(FXParticleSprite*)game->getGSM()->getFXSys()->getParticle(DirtExplosionParticleID);
	b2Vec2 vec= u->getBody()->GetPosition()-getBody()->GetPosition();
	arrow->x=u->getX();
	arrow->y=u->getY();
	game->getGSM()->getWorld()->addParticle(arrow); 

	b2World* world=game->getGSM()->getPhysics()->getWorld();
	b2AABB splashRct;
	float x=getWorldX(),y=getWorldY();
	splashRct.lowerBound.x=x-2;
	splashRct.lowerBound.y=y-2;
	splashRct.upperBound.x=x+2;
	splashRct.upperBound.y=y+2;
	SplashDamageCallback cb;
	cb.me=this;
	cb.invrange=.5f;
	cb.g=game;
	cb.damage=(strengthBase*strengthFactor+strengthIncrease)/2;
	cb.epicenter=u->getBody()->GetPosition();
	world->QueryAABB(&cb,splashRct);

	targetUnit->attackUnit(game,strengthBase*strengthFactor+strengthIncrease,this);
	if(targetUnit->isDead()){
		p->numKills++;
	}
}