#pragma once
#include "SourceCode\InGameCode\Units\Warrior.h"
#include <SourceCode\InGameCode\Unit.h>
#include "SSSF_SourceCode\gsm\physics\DynamicBody.h"
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\gsm\state\GameStateManager.h"
#include "SSSF_SourceCode\gsm\physics\Physics.h"
#include "SSSF_SourceCode\gsm\ai\BotRecycler.h"
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SourceCode\InGameCode\SensorListener.h>
#include "SSSF_SourceCode\gsm\physics\ActorHook.h"
#include "SourceCode\InGameCode\Player.h"
class TargetSensor: public SensorListener{
	Warrior* myUnit; 
public:
	TargetSensor(Warrior* owner){
		myUnit=owner;
	}
	void isWithinSensor(Unit* inRange){
		if(!myUnit->p->isAlly(inRange)){
			if(myUnit->targetUnit==NULL){
				myUnit->targetUnit=inRange;
			}
		}
	}
};