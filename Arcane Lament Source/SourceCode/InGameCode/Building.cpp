#include <SourceCode\InGameCode\Building.h>
#include <SourceCode\InGameCode\AIModule\SimpleAI.h>

class AlertUnitsCallback:public b2QueryCallback{
public:
	Unit *attackingUnit;
	Unit * me;
	bool ReportFixture(b2Fixture* fixture)
	{
		if(fixture->IsSensor())return true;
		b2Body* body = fixture->GetBody();
		ActorHook* data = (ActorHook*)body->GetUserData();
		if(data->type==UNIT_TYPE){
			Unit* inRange = (Unit*)data->data;
			if(inRange->p->isAlly(me) && inRange->targetUnit==NULL){
				inRange->targetThisUnit(attackingUnit);
			}
		}
		return true;
	}
};


void Building::attackUnit(Game* g,float damage,Unit* u){
	damageUnit(damage);
	if(alertCooldown<=0){
		b2World* world=g->getGSM()->getPhysics()->getWorld();
		b2AABB alertRct;
		float x=getWorldX(),y=getWorldY();
		alertRct.lowerBound.x=x-20;
		alertRct.lowerBound.y=y-20;
		alertRct.upperBound.x=x+20;
		alertRct.upperBound.y=y+20;
		AlertUnitsCallback cb;
		cb.attackingUnit=u;
		cb.me=this;
		world->QueryAABB(&cb,alertRct);
		alertCooldown=100;
	}
	if(state==standby)
		targetUnit=u;
}