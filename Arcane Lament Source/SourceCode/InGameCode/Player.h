#pragma once
#include "stdafx.h"
#include "SourceCode\InGameCode\Unit.h"
#include <SourceCode\InGameCode\AIModule\SimpleAI.h>
#include "SourceCode\WizardSystem\Wizard.h"

//#include "luabind\luabind.hpp" 
#include "lua.h"
//class Wizard;
class InGameState;
struct UnitEntry{
	wstring BotRecyclerID;
	vector<int> incomeIncrease;
	int imageID;
	int buildTime;
	int numToBuild;
	int currentPercent;
	int popCost,goldCost;
	wstring title;
	wstring tooltip;
	wstring titleFont;
	wstring tooltipFont;
	wstring tmp[10];
	int width;
	int titleColor[4];
	int tooltipColor[4];
};
class Player{
public:
	int numCreated,numKills, numLost;
	int goldAcrued,popAcrued;
	int numSpells;
	int manaUsed;
	int numWarriorsCreated, numWarriorsKilled,numWarriorsLost;
	int numWizardsCreated, numWizardsKilled, numWizardsLost;
	int numOgresCreated, numOgresKilled, numOgresLost;
	int numArchersCreated, numArchersKilled, numArchersLost;
	int numKnightsCreated, numKnightsKilled, numKnightsLost; 

	float goldPerPOP,pgrPerPOP;
	int id;
	int flagID, fillID, influenceID;
	float pop, tmpPop;
	float popIncome;
	int standingArmySize;
	int gold,tmpGold,goldIncome;
	float mana, maxmana, manaIncome;
	Wizard *wizard;
	float economyVsGrowthFactor;
	int aiLevel;
	Unit *tower;
	int ticks;
	int ticksMax;
	int numBarracks;
	float armyStrength;
	vector<pair<int,int>> buildStack;
	static vector<UnitEntry> unitTypes;
	SimpleAI myAI;
	Player* enemy;
	Player(){
		numCreated=0;
		numWarriorsLost=numWizardsLost=numArchersLost=numKnightsLost=numOgresLost=0;
		numKills=goldAcrued=popAcrued=numSpells=manaUsed=numWarriorsCreated=numWarriorsKilled=0;
		numWizardsCreated=numWizardsKilled=numOgresCreated=numOgresKilled=numArchersCreated=numArchersKilled=0;
		numKnightsCreated=numKnightsKilled=numLost=0;
		ticks=0;
		numBarracks=0;
		armyStrength=0;
		ticksMax=600;
		pop=100;
		flagID=0;
		fillID=0;
		gold=0;
		aiLevel=0;
		standingArmySize=0;
		manaIncome=40;
		goldIncome=0;
		maxmana=mana=250;
		pgrPerPOP=.01f;
		goldPerPOP=.835f;
		economyVsGrowthFactor=.5f;
		goldIncome=10;
		popIncome=pop*pgrPerPOP*(1-economyVsGrowthFactor);

	}

	Wizard* getWizard(void) {	return wizard;	}

	inline void setFlagID(int newID){
		flagID=newID;
	}
	inline void setFillID(int newID){
		fillID=newID;
	}
	inline void setInfluenceID(int newID){
		influenceID=newID;
	}
	inline bool isDead(){
		return tower->isDead();
	}
	inline bool isAlly(Player* p){
		return id==p->id;
	}
	inline bool isAlly(Unit* u){
		return u->p->id==id;
	}
	inline bool isEnemy(Unit* u){
		return u->p->id!=id;
	}
	inline void setAILevel(int i){
		aiLevel=i;
	}
	void queueUnit(int type, int level, int num);
	static void registerLua(lua_State* s){
		using namespace luabind;
		module(s)[
			class_<Player>("Player")
				.def("setFlagID",&setFlagID)
				.def("setFillID",&setFillID)
				.def("setInfluenceID",&setInfluenceID)
				.def("setAILevel",&setAILevel)
		];
	}
	void update(InGameState* state,Game* g);
};