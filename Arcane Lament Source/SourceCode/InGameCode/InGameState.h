#pragma once
#include <stdafx.h>
#include <SourceCode\InGameCode\Player.h>
#include "SSSF_SourceCode\game\Game.h"
#include <SSSF_SourceCode\gsm\state\GameManager.h>
#include <SourceCode\InGameCode\Units\Tower.h>
#include <SourceCode\InGameCode\Units\Warrior.h>

//#include <SourceCode\InGameCode\Spells\Spell.h>
#include <SourceCode\InGameCode\GUI\SpellDBToolbar.h>
#include <SourceCode\InGameCode\GUI\SpellToolbar.h>
#include <SourceCode\InGameCode\GUI\UnitToolbar.h>
#include <SourceCode\InGameCode\Building.h>

#include <SourceCode\InGameCode\ActionBar.h>
#include <SourceCode\InGameCode\Tools\GridExaminer.h>
#include <SourceCode\InGameCode\Tools\InfluenceMap.h>
//#include <SourceCode\InGameCode\SpellDB.h>

class GUIScreenOverlay;
class SpellDB;
class Spell;
class RenderList;
struct lua_State;
class TutorialSysGUI;
class BotRecycler;
class InGameState:public GameManager, b2ContactListener{
	vector<Building*> buildings;
	lua_State* state;
	Game* game;
	Spell *current;
	Building* currentBuilding;
	SpellButton* currentSpellBtn;
	int birthCounter;
	int gridMapCount;
	map<int,GUIScreenOverlay*> overlayWorkers;
	int popicon, goldicon;
	wstring goldstr, popstr;
	wstring fontStr;
private:
	bool shiftDown;
	SpellDB *spellDB;
	
public:	
	list<Spell*> spells;
	TutorialSysGUI* tutSys;
	vector<Player*> players;
	vector<Tower*> towers;
	int lastOverlayState;
	Player *mainPlayer;
	bool placeBuilding(Building* b,float x,float y);
	//SpellDBToolbar spDBToolbar;
	SpellToolbar toolbar;
	GridExaminer gridMap;
	bool mappable;
	int cyclicSelectInteger;
	UnitToolbar displayUnits;
	int bx , by;
	bool isCursorBusy(){
		return currentBuilding!=NULL;
	}

	vector<Building*>& getBuildings(){return buildings;}
	void setupMiscDisplayIDs();
	void setShiftDown( bool b )			{	shiftDown = b;		}
	bool getShiftDown( void )			{	return shiftDown;	}
	SpellDB* getSpellDB( void )			{	return spellDB;		}
	Player* getPlayer(void)				{	return mainPlayer;	}
	SpellToolbar& getSpellToolbar()		{	return toolbar;		}
	//SpellDB* getSpellDB( void )	{ return spellDB;	}
	int count;
	int updatePlayer();
	~InGameState(){
		//lua_close(state);
	}

	InGameState(Game* g,lua_State* setup);
	void initState(void);
	
	Player* addPlayer(int id){
		Player* p =new Player;
		p->id=id;
		players.push_back(p);
		return p;
	}

	void destroyCurrentBuilding(){
		if(currentBuilding!=NULL){
			delete currentBuilding;
			currentBuilding=NULL;
		}
	}

	void createNewBuilding(Building* newBuilding){
		destroyCurrentBuilding();
		currentBuilding=newBuilding;
	}

	bool useEscape(){
		if(currentBuilding!=NULL){
			currentBuilding->recycle(game,game->getGSM()->getSpriteManager()->getBotRecycler());
			currentBuilding=NULL;
			return true;
		}
		return false;
	}
	static void registerLua(lua_State* s);

	void createTower(string state,AnimatedSpriteType* type, int x, int y, Player *p){
		//if(!p->tower->isDead())
		//p->tower->markForRecycle();
		p->tower=new Tower();// Construct new tower at location whatever and add to game
		p->tower->setCurrentStateA(state);
		p->tower->setSpriteType(type);
		p->tower->birth(x,y,game);
		p->tower->p=p;
		this->towers.push_back((Tower*)p->tower);
		game->getGSM()->getSpriteManager()->addBot(p->tower);
	}
	void addBuilding(Building* u, int x, int y, Player*p);
	void addUnit(Unit *u,int x, int y,Player *p){
		//add unit to game
		u->p=p;		
		u->birth(x,y,game);
		game->getGSM()->getSpriteManager()->addBot(u);
		u->birthID=birthCounter++;

	}

	void lostFocus();
	bool canCastSpell(){
		return currentBuilding==NULL&& shiftDown ;
	}

	void update();
	void setMainPlayer(Player* p);
	void handleMouseInput(int x, int y);
	void handleClickInput(int x, int y,int screenX, int screenY);
	void handleRightClickInput(int x, int y,int screenX, int screenY);
	void handleClickDragInput(int iX, int iY, int eX,int eY,int screeniX,int screeniY);
	void handleGenericDragInput(int iX, int iY, int eX,int eY,int screeniX,int screeniY);
	void dispatchClickDragInput(int iX, int iY, int eX,int eY,int screeniX,int screeniY);
	void setupButtons(SpellDB* sDB);
	void render(RenderList *list, int vpx, int vpy);
	void renderGUI(RenderList *list);
	void BeginContact(b2Contact* contact);
	void EndContact(b2Contact* contact);
	void PreSolve(b2Contact* contact, const b2Manifold* oldManifold);
	void PostSolve(b2Contact* contact, const b2ContactImpulse* impulse);
	template<class T>
	void loadUnit(string type,lua_State* s, AnimatedSpriteType *atype);
	template<class T>
	void loadBuilding(string type,lua_State* s, AnimatedSpriteType *atype);
	
};