#include "SourceCode\InGameCode\Units\WizardUnit.h"
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
#include <SourceCode\InGameCode\FX\GenericFXParticles.h>
#include <SourceCode\InGameCode\InGameState.h>
#include <SourceCode\InGameCode\Units\Knight.h>
//instantiate typeid
wstring WizardUnit::typeID= L"Wizard";
float WizardUnit::maxSpeedBase=2;
float WizardUnit::attackSpeedBase=4;
float WizardUnit::maxHealthBase=50;
float WizardUnit::armorBase=50;
float WizardUnit::frictionBase=.05f;
float WizardUnit::attackRangeBase=50;
float WizardUnit::sizeBase=10;
float WizardUnit::aggroRange=10;
float WizardUnit::unitMass=10;
float WizardUnit::strengthBase=5;
float WizardUnit::healPower=5;

float WizardUnit::hpScaleX,WizardUnit::hpScaleY;
float WizardUnit::hpOffX,WizardUnit::hpOffY;

int WizardUnit::lastHealCooldown=0;

WizardUnit::WizardUnit(){
	state=standby;
	setVisible(true);
	setAlpha(255);
	targetUnit=targetTower=NULL;
	hook=new ActorHook();
	targetInRange=0;
	callback.myUnit=this;
	lastHeal=0;
	level=1;
}

void WizardUnit::isWithinAttackRange(Unit * inRange){
	if(!p->isAlly(inRange)){
		if(targetInRange==0){
			targetUnit=inRange;
			targetInRange=1;
			//Set State to Attack
			attackCooldown=attackSpeedBase*attackSpeedFactor;
		}
	}
}
void WizardUnit::leavesRange(Unit * inRange){
}
void WizardUnit::leavesAttackRange(Unit * inRange){
		if(inRange==targetUnit){
			targetInRange=0;
			//Cant attack, set state to walk
		}
	
}
void WizardUnit::determineWayPoint(InGameState* state,Game* game,float x, float y){
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
			((WizardUnit*)self)->setTargetUnit(other);
			return false;
		}else return true;
	}
};

void WizardUnit::loadSelf(lua_State* s){
	luabind::object table = luabind::globals(s)["UnitTypeConstants"]["Wizard"];
	lastHealCooldown=luabind::object_cast<float>(table["lastHealCooldown"]);
	healPower=luabind::object_cast<float>(table["healPower"]);
}

class HealUnitCallback:public b2QueryCallback{
public:
	Game* g;
	Unit * me;
	Unit * lowest;
	float percentage;
	bool ReportFixture(b2Fixture* fixture)
	{
		if(fixture->IsSensor())return true;
		b2Body* body = fixture->GetBody();
		ActorHook* data = (ActorHook*)body->GetUserData();
		if(data->type==UNIT_TYPE){
			Unit* inRange = (Unit*)data->data;
			if(inRange!=me && me->p->isAlly(inRange) && !inRange->isDead()){
				float percent = inRange->hp/inRange->getMaxHealth();
				if(percent<percentage){
					percentage=percent;
					lowest=inRange;
				}
			}
		}
		return true;
	}
};

void WizardUnit::healNearbyUnits(Game* game){
	HealUnitCallback  cb;
	cb.g=game;
	cb.lowest=NULL;
	cb.percentage=1;
	cb.me=this;
	targetArea.upperBound-=targetArea.lowerBound;
	targetArea.lowerBound.x=getBody()->GetPosition().x-targetArea.upperBound.x/2.0f;
	targetArea.lowerBound.y=getBody()->GetPosition().y-targetArea.upperBound.y/2.0f;
	targetArea.upperBound+=targetArea.lowerBound;
	game->getGSM()->getPhysics()->getWorld()->QueryAABB((&cb),targetArea);
	if(cb.lowest!=NULL){
		Unit* u = cb.lowest;
		u->hp+=healPower;
		FXParticle* p= game->getGSM()->getFXSys()->getParticle(SparkleParticleID);
		p->x=u->getX();
		p->y=u->getY();
		game->getGSM()->getWorld()->addParticle(p);
	}
}

bool WizardUnit::think(Game *game){
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

		if(lastHeal--<0){
			healNearbyUnits(game);
			lastHeal=lastHealCooldown;
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
				b2AABB targetaabb = targetUnit->getAABB();
				closeArea.lowerBound.x=getBody()->GetPosition().x-closeArea.upperBound.x/2.0f;
				closeArea.lowerBound.y=getBody()->GetPosition().y-closeArea.upperBound.y/2.0f;
				closeArea.upperBound+=closeArea.lowerBound;
				if(b2TestOverlap(targetaabb,closeArea)){	
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
			
			
			lastThink=15;
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

void WizardUnit::handleOrders(Game* game){
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
				closeArea.upperBound-=closeArea.lowerBound;
				b2AABB targetaabb = targetUnit->getAABB();
				closeArea.lowerBound.x=getBody()->GetPosition().x-closeArea.upperBound.x/2.0f;
				closeArea.lowerBound.y=getBody()->GetPosition().y-closeArea.upperBound.y/2.0f;
				closeArea.upperBound+=closeArea.lowerBound;
				if(!b2TestOverlap(targetaabb,closeArea))
					determineWayPoint(manager,game,targetUnit->getWorldX(),targetUnit->getWorldY());
				else{
					waypoints.clear();
					lastWaypointGeneration=10;
				}
			}
			else lastWaypointGeneration=10;
}
void WizardUnit::recycle(Game* game,BotRecycler * recycle){
	game->getGSM()->getPhysics()->destroyBody(this);
	recycle->recycleBot(WizardUnit::typeID,this);
	targetUnit=NULL;
	targetInRange=0;
	state=standby;
	setCurrentState((wstring)L"IDLE_STATE_NORTH");
	this->generalRelease();
}
void WizardUnit::isWithinRange(Unit* inRange){
	if(!p->isAlly(inRange)){ 
			if(targetUnit==NULL){
				targetUnit=inRange;
			}
		}
}


class LineDamageCallback:public b2RayCastCallback{
public:
	Game* g;
	WizardUnit * me;
	float damage;
	int split;
	b2Vec2 myDirection;
	 float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
									  const b2Vec2& normal, float32 fraction)
	{	
		if(fixture->IsSensor())return true;
		b2Body* body = fixture->GetBody();
		ActorHook* data = (ActorHook*)body->GetUserData();
		if(data->type==UNIT_TYPE){
			Unit* inRange = (Unit*)data->data;
			if(inRange!=me && !me->p->isAlly(inRange)){
				if(me->level>=1){
					inRange->attackUnit(g,damage,me);
					if(inRange->isDead()){
						me->p->numKills++;
					}
					if(split>0&& rand()%100<20*(me->level+1)){
						for(int i=0;i<5;i++){
							b2Vec2 newDir= myDirection;
							newDir.x+=rand()%16-8;
							newDir.y+=rand()%16-8;
							newDir.Normalize();
							newDir*=15;
							FXParticle *arrow=g->getGSM()->getFXSys()->getParticle(LightningParticleID);
							arrow->r=atan2(newDir.y,newDir.x);
							arrow->sX*=newDir.Length();
							arrow->x=inRange->getX();
							arrow->y=inRange->getY();
							//arrow->offX=-500;
							g->getGSM()->getWorld()->addParticle(arrow);
							b2World* world=g->getGSM()->getPhysics()->getWorld();
							LineDamageCallback cb;
							cb.me=me;
							cb.g=g;
							cb.myDirection=newDir;
							cb.split=split-1;
							cb.damage=damage*.333f;
							b2Vec2 tmp=inRange->getBody()->GetPosition();
							tmp+=newDir;
							world->RayCast(&cb,inRange->getBody()->GetPosition(),tmp);
						}
					}
				}else{
					inRange->attackUnit(g,damage,me);
					if(inRange->isDead()){
						me->p->numKills++;
					}
				}
			}
		}
		return -1.0f;
	}
};

void WizardUnit::onAttack(Game* game, Unit* u, float damage){
	FXParticle *arrow=game->getGSM()->getFXSys()->getParticle(LightningParticleID);
	b2Vec2 vec= u->getBody()->GetPosition()-getBody()->GetPosition();
	arrow->r=atan2(vec.y,vec.x);
	arrow->sX*=vec.Length();
	arrow->x=getX();
	arrow->y=getY();
	//arrow->offX=-500;
	game->getGSM()->getWorld()->addParticle(arrow);
	b2World* world=game->getGSM()->getPhysics()->getWorld();
	LineDamageCallback cb;
	cb.me=this;
	cb.g=game;
	cb.myDirection=vec;
	cb.split=level;
	cb.damage=(strengthBase*strengthFactor+strengthIncrease)*.333f;
	world->RayCast(&cb,getBody()->GetPosition(),targetUnit->getBody()->GetPosition());
	//game->getGSM()->getPhysics()->getWorld()->RayCast(&cb,getBody()->GetPosition(),targetUnit->getBody()->GetPosition());
	if(typeid(*targetUnit)==typeid(Knight)){
		targetUnit->attackUnit(game,(strengthBase*strengthFactor+strengthIncrease)*1.5f,this);
	}else
		targetUnit->attackUnit(game,strengthBase*strengthFactor+strengthIncrease*.666f,this);
	if(targetUnit->isDead()){
		p->numKills++;
	}
}