#include "SourceCode\InGameCode\Units\Knight.h"
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
#include <SourceCode\InGameCode\Units\Archer.h>
//instantiate typeid
wstring Knight::typeID= L"Knight";
float Knight::maxSpeedBase=2;
float Knight::attackSpeedBase=4;
float Knight::maxHealthBase=50;
float Knight::armorBase=50;
float Knight::frictionBase=.05f;
float Knight::attackRangeBase=50;
float Knight::sizeBase=10;
float Knight::aggroRange=10;
float Knight::unitMass=10;
float Knight::strengthBase=5;
float Knight::hpScaleX,Knight::hpScaleY;
float Knight::hpOffX,Knight::hpOffY;

Knight::Knight(){
	state=standby;
	setVisible(true);
	setAlpha(255);
	targetUnit=targetTower=NULL;
	hook=new ActorHook();
	targetInRange=0;
	
	callback.myUnit=this;
}

void Knight::isWithinAttackRange(Unit * inRange){
	if(!p->isAlly(inRange)){
		if(targetInRange==0){
			targetUnit=inRange;
			targetInRange=1;
			//Set State to Attack
			attackCooldown=attackSpeedBase*attackSpeedFactor;
		}
	}
}
void Knight::leavesRange(Unit * inRange){
}
void Knight::leavesAttackRange(Unit * inRange){
		if(inRange==targetUnit){
			targetInRange=0;
			//Cant attack, set state to walk
		}
	
}
void Knight::determineWayPoint(InGameState* state,Game* game,float x, float y){
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

class CloseRange:public CallbackFunctor{
public:
	bool operator()(Unit* self, Unit* other){
		if(!self->p->isAlly(other)){
			((Knight*)self)->setTargetUnit(other);
			return false;
		}else return true;
	}
};

void Knight::loadSelf(lua_State* s){
}

bool Knight::think(Game *game){
		if(alertCooldown>0)alertCooldown--;
		if(attackCooldown>0)attackCooldown--;
		if(isDead() && this->getCurrentState()==DEATH_STATE && justStartedState())
			return true;
		else if(isDead() && this->getCurrentState()!=DEATH_STATE){
			this->setCurrentState((wstring)DEATH_STATE);
			body->SetActive(false);
		}
		else if (isDead())
			return false;
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
			if(justStartedState()){
					this->onAttack(game,targetUnit,strengthBase*strengthFactor+strengthIncrease);
					state=standby;
					attackCooldown=attackSpeedBase*attackSpeedFactor; 
			}
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
					if(targetUnit!=NULL)lastWaypointGeneration=0;
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

void Knight::handleOrders(Game* game){
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
void Knight::recycle(Game* game,BotRecycler * recycle){
	game->getGSM()->getPhysics()->destroyBody(this);
	recycle->recycleBot(Knight::typeID,this);
	targetUnit=NULL;
	targetInRange=0;
	state=standby;
	setCurrentState((wstring)L"IDLE_STATE_NORTH");
	this->generalRelease();
}
void Knight::isWithinRange(Unit* inRange){
	if(!p->isAlly(inRange)){ 
			if(targetUnit==NULL){
				targetUnit=inRange;
			}
		}
}

void Knight::onAttack(Game* game, Unit* u, float damage){
	FXParticleSprite *arrow=(FXParticleSprite*)game->getGSM()->getFXSys()->getParticle(SlashParticleID);
	b2Vec2 vec= u->getBody()->GetPosition()-getBody()->GetPosition();
	arrow->r=atan2(vec.y,vec.x);
	arrow->x=getX()+8*cos(arrow->r);
	arrow->y=getY()+8*sin(arrow->r);
	game->getGSM()->getWorld()->addParticle(arrow); 
	if(typeid(*targetUnit)==typeid(Warrior)|| typeid(*targetUnit)==typeid(Archer)){
		targetUnit->attackUnit(game,(strengthBase*strengthFactor+strengthIncrease)*1.5f,this);
	}else
		targetUnit->attackUnit(game,strengthBase*strengthFactor+strengthIncrease,this);
	
	if(targetUnit->isDead()){
		p->numKills++;
	}
}