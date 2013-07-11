#pragma once
#include <stdafx.h>
class Unit;
class InGameState;
class Game;
class Player;
class Building;
class Tower;
class Barracks;
struct Cluster{
	bool friendly;
	Player* owner;
	float x,y;
	float radius;
	float strength;
	float vX,vY;
};


struct Base{
	float x, y;
	float range;
	float priority;
	vector<Building*> buildings;
	Player* owner;
	int n;
};
struct Stats{
	float productionCoeff;
	float magicCoeff;
};
struct Priorities{
	float expandPriority,
		militaryPriority,
		productionPriority,
		resourcePriority,
		attackPriority,
		defensePriority;
	int numWarriors,
		numArchers,
		numWizards,
		numKnights,
		numOgres;
};
struct Task{

};
class SimpleAI{
	int lastCreateClusters;
	vector<Cluster> existingClusters;
	vector<Base> myBases, enemyBases;
	vector<Barracks*> myBarracks;
	map<Player*, Stats> statsMap;
	Priorities myPriorities;
	vector<int> tasks;
	list<Unit*> myArmy;
	int priorityOrder;
public:
	vector<Base>& getMyBases(){return myBases;}
	vector<Base>& getEnemyBases(){return enemyBases;}
	vector<Cluster>& getClusters(){return existingClusters;}
	SimpleAI(){lastCreateClusters=1000;}
	void countUnitsAndDispatchOrders(InGameState *s, Game* g, Player* me);
	void constructClusters(InGameState *s, Game* g, Player* me);
	void checkClusters(InGameState *s, Game* g, Player* me);
	void think(InGameState *s, Game* g, Player* me);
}; 