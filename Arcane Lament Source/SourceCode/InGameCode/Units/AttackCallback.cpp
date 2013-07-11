#include <Box2D\Box2D.h>
#include "stdafx.h"
#include <SourceCode\InGameCode\Units\AttackCallback.h>
#include <SourceCode\InGameCode\Unit.h>
#include "SSSF_SourceCode\gsm\physics\ActorHook.h"
#include <SourceCode\InGameCode\Player.h>
//#include <SourceCode\InGameCode\Units\Warrior.h>
//#include <SourceCode\InGameCode\Units\Ogre.h>
bool AttackCallback::ReportFixture(b2Fixture* fixture)
{
	if(fixture->IsSensor())return true;
	b2Body* body = fixture->GetBody();
	ActorHook* data = (ActorHook*)body->GetUserData();
	if(data->type==UNIT_TYPE||data->type==BUILDING_TYPE){
		Unit* inRange = (Unit*)data->data;
		if(!myUnit->p->isAlly(inRange)){
			if(myUnit->targetUnit==NULL){
				myUnit->setTargetUnit(inRange);
				return false;
			}
		}
	}
	return true;
}