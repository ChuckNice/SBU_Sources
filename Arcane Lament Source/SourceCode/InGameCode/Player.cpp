#include <SourceCode\InGameCode\Player.h>
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <SourceCode\InGameCode\InGameState.h>
#include <SourceCode\InGameCode\GUI\TutorialSysGUI.h>
void Player::update(InGameState* state,Game* g){
	if(aiLevel!=0)
		myAI.think(state,g,this); 
	if(ticks++>max(ticksMax,300)){
		ticksMax--;
		mana+=manaIncome;
		if(mana>maxmana)mana=maxmana;
		gold+=goldIncome;
		while(!buildStack.empty()){
			int level = buildStack[0].first;
			int type = buildStack[0].second;
			Unit* u = (Unit*)g->getGSM()->getSpriteManager()->getBotRecycler()->retrieveBot(unitTypes[type].BotRecyclerID);
			state->addUnit(u,tower->getX()+(float)(rand()%50-25)/50*5,tower->getY()+(float)(rand()%50-25)/50*5,this);
			u->issueAttackToOrder(enemy->tower->getWorldX(),enemy->tower->getWorldY());
			buildStack.erase(buildStack.begin());
		}
		goldAcrued+=goldIncome;
		ticks=0;
		
		//Testing/ cheating
		/*
		for(int i=0;i<5;i++)buildStack.push_back(make_pair(0,1));
		for(int i=0;i<5;i++)buildStack.push_back(make_pair(0,2));
		for(int i=0;i<5;i++)buildStack.push_back(make_pair(0,3));
		for(int i=0;i<10;i++)buildStack.push_back(make_pair(0,0));
		*/
	}
	
}

void Player::queueUnit(int type, int level, int num){
	for(int i=0;i<num;i++){
		if(gold>=unitTypes[type].goldCost && buildStack.size()<80){
			buildStack.push_back(make_pair(level,type));
			gold-=unitTypes[type].goldCost;
			goldIncome+=unitTypes[type].incomeIncrease[level];
		}
	}
}
vector<UnitEntry> Player::unitTypes;