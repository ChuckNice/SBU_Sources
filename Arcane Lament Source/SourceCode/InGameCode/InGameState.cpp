#include <SourceCode\InGameCode\InGameState.h>
#include <SSSF_SourceCode\gsm\sprite\AnimatedSpriteType.h>
#include "SSSF_SourceCode\game\Game.h"
#include <SourceCode\InGameCode\New folder\UnitHPBar.h>
#include <SourceCode\InGameCode\Units\Warrior.h>
#include <SourceCode\InGameCode\Units\Archer.h>
#include <SourceCode\InGameCode\Units\Knight.h>
#include <SourceCode\InGameCode\Units\WizardUnit.h>
#include <SourceCode\InGameCode\Units\Ogre.h>
#include <SourceCode\LuaModules\GameRendererModule.h>
#include <SourceCode\InGameCode\SensorListener.h>
#include <SSSF_SourceCode\gui\Cursor.h>

#include <SourceCode\InGameCode\Spells\Fireball.h>
#include <SourceCode\InGameCode\Spells\GravityWell.h>
#include <SourceCode\InGameCode\Spells\GravityBomb.h>
#include <SourceCode\InGameCode\Spells\FlameWall.h>
#include <SourceCode\InGameCode\Spells\Meteor.h>
#include <SourceCode\InGameCode\Spells\MeteorShower.h>
#include <SourceCode\InGameCode\Spells\KineticBooster.h>

#include <SourceCode\InGameCode\StatusFX\SelectedFX.h>
#include <SourceCode\InGameCode\FX\GenericFXParticles.h>

#include <SourceCode\InGameCode\GUI\SpellToolbar.h>
#include <SourceCode\InGameCode\GUI\SpellDBToolbar.h>
#include <SourceCode\InGameCode\ActionBar.h>
#include <SourceCode\InGameCode\SpellDB.h>
#include <SourceCode\InGameCode\Building.h>
#include <SourceCode\ProjectBase\ResourceBank.h>
#include <SourceCode\InGameCode\GUI\GUIScreenOverlay.h>
#include <SourceCode\InGameCode\GUI\TownHallGUI.h>
#include <SourceCode\InGameCode\GUI\TutorialSysGUI.h>
#include <SourceCode\InGameCode\GUI\SpellSelectionGUI.h>
#include <SourceCode\InGameCode\GUI\StatsScreen.h>
#include <SourceCode\InGameCode\GUI\BattleReportGUI.h>
#include <SourceCode\InGameCode\GUI\BarracksGUI.h>
#include <SourceCode\WizardSystem\Wizard.h>
#include <SourceCode\InGameCode\AIModule\SimpleAI.h>
#include <SourceCode\InGameCode\Units\Building\Barracks.h>
#include <SourceCode\InGameCode\Units\Building\Altar.h>
#include <SourceCode\InGameCode\Units\Building\Farm.h>
#include <SourceCode\InGameCode\Units\Building\ScoutTower.h>
#include <SSSF_SourceCode\sound\SoundManager.h>

#include "luabind\luabind.hpp"
#include "lua.h"

bool sortByZPred(Bot* b1, Bot* b2){
	bool flag = b1->getZ()<b2->getZ();
	return flag; 
}

InGameState::InGameState(Game* g,lua_State* setup):GameManager(g),
	toolbar(270,630,AVAILABLE_SLOTS*50,MAX_TOOLBARS*50,50,50),gridMap(g->getGSM()->getWorld(),TILE_WIDTH,TILE_HEIGHT),
	displayUnits(600,630,250,150,50,50,100){

	registerLua(setup);
	luabind::globals(setup)["gamestate"]=this;
	ResourceBankRegisterLua(setup); 
	Player::registerLua(setup);
	AnimatedSpriteType::registerLua(setup);
	//SpellDB::registerLua(setup);			///here
	players.clear();
	state=setup;
	game=g;
	currentBuilding=NULL;
	shiftDown=false;
	string err="";
	Unit::cyclicSelect=true;
	cyclicSelectInteger=0;
	if(luaL_dostring(setup,"setup()")==1) 
		err="Couldn't run Lua program: "+(string)lua_tostring(state, -1);
	cout<<err;
	mainPlayer=players[0];
	//Setup contact Listener
	game->getGSM()->getPhysics()->setContactListener(this);

	//Load in FX 
	game->getGSM()->getFXSys()->clearSystem();
	loadParticles(game,setup);

	displayUnits.bgID=ResourceBankgetResource((string)"ToolTipBG");

	//HERE CONTROL RETURNS TO THE CALLER WHO WILL IMMEDIATELY 
	//INITIALIZE THE InGameState AS BELOW
	

}


void InGameState::initState(){
	
	//Load in unit hp bar
	UnitHPBar::container=luabind::object_cast<int>(luabind::globals(state)["HPBARCONTAINERTEXTUREID"]);


	
	BotRecycler* recycler=game->getGSM()->getSpriteManager()->getBotRecycler();
	string err="";

	game->getGUI()->getCursor(GS_GAME_IN_PROGRESS)->setActiveCursorID(0);
	//Load in Warrior Unit
	Warrior *w = new Warrior();
	w->setSpriteType(luabind::call_function<AnimatedSpriteType*>(state,"getWarriorSpriteType"));
	w->setCurrentState((wstring)L"IDLE_STATE_NORTH");
	this->loadUnit<Warrior>("Warrior",state,w->getSpriteType());
	recycler->registerBotType(L"WARRIOR",w);
	recycler->initRecyclableBots(L"WARRIOR",600);
	
	//Load in Archer Unit
	Archer *arch = new Archer();
	arch->setSpriteType(game->getGSM()->getSpriteManager()->getSpriteType("ArcherSprite"));
	arch->setCurrentState((wstring)L"IDLE_STATE_NORTH");
	this->loadUnit<Archer>("Archer",state,arch->getSpriteType());
	recycler->registerBotType(L"Archer",arch);
	recycler->initRecyclableBots(L"Archer",300);

	//Load in Wizard Unit
	WizardUnit *wiz = new WizardUnit();
	wiz->setSpriteType(game->getGSM()->getSpriteManager()->getSpriteType("WizardSprite"));
	wiz->setCurrentState((wstring)L"IDLE_STATE_NORTH");
	this->loadUnit<WizardUnit>("Wizard",state,wiz->getSpriteType());
	recycler->registerBotType(L"Wizard",wiz);
	recycler->initRecyclableBots(L"Wizard",300);

	//Load in Knight Unit
	Knight *knight = new Knight();
	knight->setSpriteType(game->getGSM()->getSpriteManager()->getSpriteType("KnightSprite"));
	knight->setCurrentState((wstring)L"IDLE_STATE_NORTH");
	this->loadUnit<Knight>("Knight",state,knight->getSpriteType());
	recycler->registerBotType(L"Knight",knight);
	recycler->initRecyclableBots(L"Knight",300);

	//LOAD IN THE OGRE UNIT
	Ogre *o = new Ogre();
	o->setSpriteType(luabind::call_function<AnimatedSpriteType*>(state,"getOgreSpriteType"));
	o->setCurrentState((wstring)L"IDLE_STATE_NORTH");
	this->loadUnit<Ogre>("Ogre",state,o->getSpriteType());
	recycler->registerBotType(L"OGRE",o);
	recycler->initRecyclableBots(L"OGRE",30);

	
	Barracks* tmp= new Barracks();
	tmp->setSpriteType(game->getGSM()->getSpriteManager()->getSpriteType("BarracksSprite"));
	this->loadBuilding<Barracks>("Barracks",state,tmp->getSpriteType());
	recycler->registerBotType(L"Barracks",tmp);
	recycler->initRecyclableBots(L"Barracks",10);

	Altar* altar= new Altar();
	altar->setSpriteType(game->getGSM()->getSpriteManager()->getSpriteType("AltarSprite"));
	this->loadBuilding<Altar>("Altar",state,altar->getSpriteType());
	recycler->registerBotType(L"Altar",altar);
	recycler->initRecyclableBots(L"Altar",10);

	
	Farm* farm= new Farm();
	farm->setSpriteType(game->getGSM()->getSpriteManager()->getSpriteType("FarmSprite"));
	this->loadBuilding<Farm>("Farm",state,farm->getSpriteType());
	recycler->registerBotType(L"Farm",farm);
	recycler->initRecyclableBots(L"Farm",10);

	
	ScoutTower* scouttower= new ScoutTower();
	scouttower->setSpriteType(game->getGSM()->getSpriteManager()->getSpriteType("ScoutTowerSprite"));
	this->loadBuilding<ScoutTower>("ScoutTower",state,scouttower->getSpriteType());
	recycler->registerBotType(L"ScoutTower",scouttower);
	recycler->initRecyclableBots(L"ScoutTower",10);



	Tower::loadSelf(state);	
	err="";
	if(luaL_dostring(state,"createUnits()")==1) 
		err="Couldn't run Lua program: "+(string)lua_tostring(state, -1);
	cout<<err;
	Tower::hpScaleX=Tower::hpScaleY=(float)mainPlayer->tower->getSpriteType()->getTextureWidth() /128;
	Tower::hpOffX=0;
	Tower::hpOffY=32*Tower::hpScaleY;
	


	Player::unitTypes.clear();
	for(luabind::iterator i(luabind::globals(state)["UnitEntriesDatabase"]),end;i!=end;++i){
		UnitEntry newType;
		int x=1;
		newType.BotRecyclerID=convertwstrX(luabind::object_cast<string>(i->operator[](x++)));
		newType.imageID=ResourceBankgetResource(luabind::object_cast<string>(i->operator[](x++)));
		newType.incomeIncrease.push_back(luabind::object_cast<int>(i->operator[](x++)));
		newType.buildTime=luabind::object_cast<int>(i->operator[](x++));
		newType.popCost=luabind::object_cast<int>(i->operator[](x++));
		newType.goldCost=luabind::object_cast<int>(i->operator[](x++));
		newType.title=convertwstrX(luabind::object_cast<string>(i->operator[](x++)));
		newType.tooltip=convertwstrX(luabind::object_cast<string>(i->operator[](x++)));
		newType.titleFont=convertwstrX(luabind::object_cast<string>(i->operator[](x++)));
		newType.tooltipFont=convertwstrX(luabind::object_cast<string>(i->operator[](x++)));
		newType.width=luabind::object_cast<int>(i->operator[](x++));
		for(int k=0;k<4;k++)newType.titleColor[k]=luabind::object_cast<int>(i->operator[](x++));
		for(int k=0;k<4;k++)newType.tooltipColor[k]=luabind::object_cast<int>(i->operator[](x++));
		newType.numToBuild=0;
		newType.currentPercent=0;
		Player::unitTypes.push_back(newType);
	}





	//SETUP SPELL DB
	spellDB = new SpellDB();
	spellDB->loadDB(game, state);

	mainPlayer->wizard = new Wizard();
	
	//Setup Spells 
	Fireball::displayId=luabind::object_cast<int>(luabind::globals(state)["FIREBALLPROJECTILEID"]);
	FlameWall::displayIds.clear();
	for(luabind::iterator i(luabind::globals(state)["FLAMEWALLIDS"]),end;i!=end;++i)
		FlameWall::displayIds.push_back(luabind::object_cast<int>(*i));
	GravityWell::displayIds.clear();
	for(luabind::iterator i(luabind::globals(state)["GRAVITYWELLIDS"]),end;i!=end;++i)
		GravityWell::displayIds.push_back(luabind::object_cast<int>(*i));
	GravityBomb::displayIds.clear();
	for(luabind::iterator i(luabind::globals(state)["GRAVITYBOMBIDS"]),end;i!=end;++i)
		GravityBomb::displayIds.push_back(luabind::object_cast<int>(*i));
	Meteor::displayId=luabind::object_cast<int>(luabind::globals(state)["METEORID"]);
	Meteor::shadowId=luabind::object_cast<int>(luabind::globals(state)["METEORSHADOWID"]);
	MeteorShower::displayId=luabind::object_cast<int>(luabind::globals(state)["METEORID"]);
	MeteorShower::shadowId=luabind::object_cast<int>(luabind::globals(state)["METEORSHADOWID"]);
	KineticBooster::displayId=luabind::object_cast<int>(luabind::globals(state)["KINETICBOOSTERID"]);
	SpellDB::BUTTON_SOCKET=luabind::object_cast<int>(luabind::globals(state)["SPELLBUTTONSOCKETID"]);	

	Spell::SpellArrowID=game->getResManager()->getImageIDLua("$$SpellArrow",false);
	Spell::SpellCircleID=game->getResManager()->getImageIDLua("$$SpellCircle",false);
	Spell::SpellLineID=game->getResManager()->getImageIDLua("$$SpellLine",false);
	Spell::SpellAOEID=game->getResManager()->getImageIDLua("$$SpellAOE",false);

	//SETUP BUTTONS
	toolbar.initializeToolbar(game);
	displayUnits.initializeToolbar(this);
	//setupButtons(spellDB);
	this->setupMiscDisplayIDs();

	SpellSelectionGUI *ssg = new SpellSelectionGUI();
	ssg->setupScreen(game, state, spellDB);
	overlayWorkers[SPELL_SELECTION_GUI] = ssg;

	StatsScreen *ss = new StatsScreen();
	ss->onStart(game); //currently initializes the screen and its vars
	overlayWorkers[STATS_SCREEN] = ss;

	count=0;
	spells.empty();
	 TutorialSysGUI* tsGUI = new TutorialSysGUI();
	 tsGUI->setupSys(this,game,state);
	 overlayWorkers[TUTSYSSCREENOVERLAYNUM]=tsGUI;
	 tsGUI->currentTutorial="StartGame";
	 this->tutSys=tsGUI;


	popicon=ResourceBankgetResource((string)"PopIcon");
	goldicon=ResourceBankgetResource((string)"GoldIcon");
	gridMapCount=0;
	fontStr=L"Times New Roman";


	
	players[1]->enemy=players[0];
	players[0]->enemy=players[1];
//if(!mainPlayer->wizard->startGameTutorial){
//
//			game->getGSM()->overlayGameState=TUTSYSSCREENOVERLAYNUM;
//			tutSys->currentTutorial="StartGame";
//			mainPlayer->wizard->startGameTutorial=true;
//		}
	//Cheats
	
	//mainPlayer->gold=10000;
	//mainPlayer->pop=1000;
	
	//players[1]->gold=10000;
	//players[1]->pop=1000;
}


void InGameState::lostFocus(){
		if(currentBuilding!=NULL){
			destroyCurrentBuilding();
		}
	}


void InGameState::setupButtons(SpellDB *sDB){
	// UPDATE TOOLBAR AFTER SPELL SELECTION

	//@TODO: HERE WE WILL SET UP THE SPELL TOOLBAR BASED ON THE USER SELECTED 
	// SPELLD FROM HIS REPETOIRE(?)
	
	//toolbar.addSpell(sDB->getSpellButton("Fireball"), sDB->getSpell("Fireball"));
	//......
}


//Performs insertion sort on a list of bots.
void InsertionSort(list<Bot*>* botList){
	list<Bot*>::iterator start, end, innerStart, innerEnd,beginning, temp;
		start=botList->begin();
		end= botList->end();
		innerEnd= botList->begin();
		bool flag=false,swap=false;
		for(;start!=end;++start){
			flag=false;
			if(start!=innerEnd){
				innerStart=start;
				innerStart--;
				swap=false;
				for(;(*innerStart)->getZ()>(*start)->getZ();--innerStart){
					swap=true;
					if(innerStart==innerEnd){
						temp = start++;
						botList->splice(innerStart,*botList,temp);
						innerEnd=botList->begin();
						flag=true;
						break;
					}
				}
				if(!flag && swap){
					temp = start++;
					botList->splice(++innerStart,*botList,temp);
				}
			}
		}
}


int InGameState::updatePlayer(){
	vector<Player*>::iterator start=players.begin(), end=players.end();
	int deadPlayer = 0;
	for(;start!=end;start++,deadPlayer++){
		(*start)->update(this,game);
		if ( players.at(deadPlayer)->isDead() )
			return deadPlayer;
	}
	return -1;
}
void InGameState::update(){
	if(game->getGSM()->getOverlayGameState()!=lastOverlayState){
		map<int,GUIScreenOverlay*>::iterator find=overlayWorkers.find(lastOverlayState);
		if(find!=overlayWorkers.end())(*find).second->onEnd();
		lastOverlayState=game->getGSM()->getOverlayGameState();
		find=overlayWorkers.find(lastOverlayState);
		if(find!=overlayWorkers.end())(*find).second->onStart();
	}
	if(!game->getGSM()->isGameInProgress()){
		int overlayState= game->getGSM()->getOverlayGameState();
		map<int,GUIScreenOverlay*>::iterator find=overlayWorkers.find(overlayState);
		if(find!=overlayWorkers.end())(*find).second->update(game);
		return;
	}

	if(!mainPlayer->wizard->startGameTutorial){
		game->getGSM()->overlayGameState=TUTSYSSCREENOVERLAYNUM;
		tutSys->currentTutorial="StartGame";
		mainPlayer->wizard->startGameTutorial=true;
	}

	/** sort the bots to make rendering more realistic **/
	//Using insertion sort to keep bots in y order. Is poor in high inversion counts. Degrades to 30 FPS at ~608 bodies with test sim.
	//Without sort 650 bodies? depends on the situation. Should be good enough to use.
	InsertionSort(game->getGSM()->getSpriteManager()->getBotList());
	if(gridMapCount--<=0){gridMap.evaluateGrid(game);gridMapCount=120;}


	//GAME OVER HANDLING HERE
	int deadPlayer;
	if((deadPlayer = updatePlayer()) >= 0){

		BattleReportGUI *brg = new BattleReportGUI();			

		if(deadPlayer == 0){
			//DISPLAY LOSS
			brg->onStart(game, false);
			overlayWorkers[GS_GAME_OVER] = brg;
			game->getSoundManager()->stopCue("MAIN_LOOP");
			game->getSoundManager()->playCue("VICTORY");
			game->getGSM()->gameOver();
		}else{
			//DISPLAY VICTORY
			brg->onStart(game, true);
			overlayWorkers[GS_VICTORY] = brg;
			game->getSoundManager()->stopCue("MAIN_LOOP");
			game->getSoundManager()->playCue("VICTORY");
			game->getGSM()->victory();
		}
	}



	list<Spell*>::iterator spellIt= spells.begin();
	while(spellIt!=spells.end()){
		Spell* spell = *spellIt;
		if(spell->update(game)){
			spellIt=spells.erase(spellIt);
			delete spell;
		}
		else
			spellIt++;
	}
	if(count++>100 && !players[0]->tower->isDead()&& !players[1]->tower->isDead()){
		BotRecycler* recycler=game->getGSM()->getSpriteManager()->getBotRecycler();
		Unit* u;


		count=0;
	}
}


void InGameState::BeginContact(b2Contact* contact)
{
	b2Fixture *a = contact->GetFixtureA(),
		*b=contact->GetFixtureB();
	if(contact->IsTouching()){
		if(a->IsSensor()&&!b->IsSensor()){
			ActorHook *bData=(ActorHook*)b->GetBody()->GetUserData();
			if(bData==NULL)return;
			if(bData->type==UNIT_TYPE){
				SensorFunctor *aListener= (SensorFunctor*)a->GetUserData();
				(*aListener).enterCall((Unit*)bData->data);
			}
		}
		else if(b->IsSensor()&&!a->IsSensor()){
			ActorHook *aData=(ActorHook*)a->GetBody()->GetUserData();
			if(aData==NULL)return;
			if(aData->type==UNIT_TYPE){
				SensorFunctor *bListener= (SensorFunctor*)b->GetUserData();
				(*bListener).enterCall((Unit*)aData->data);
			}
		}
	}
}


void InGameState::EndContact(b2Contact* contact)
{
	b2Fixture *a = contact->GetFixtureA(),
		*b=contact->GetFixtureB();
	if(contact->IsTouching()){
		if(a->IsSensor()&&!b->IsSensor()){
			ActorHook *bData=(ActorHook*)b->GetBody()->GetUserData();
			if(bData==NULL)return;
			if(bData->type==UNIT_TYPE){
				SensorFunctor *aListener= (SensorFunctor*)a->GetUserData();
				(*aListener).leaveCall((Unit*)bData->data);
			}
		}
		else if(b->IsSensor()&&!a->IsSensor()){
			ActorHook *aData=(ActorHook*)a->GetBody()->GetUserData();
			if(aData==NULL)return;
			if(aData->type==UNIT_TYPE){
				SensorFunctor *bListener= (SensorFunctor*)b->GetUserData();
				(*bListener).leaveCall((Unit*)aData->data);
			}
		}
	}
}

float calculateApproachVelocity(b2Contact* contact){
	b2WorldManifold worldManifold;
	contact->GetWorldManifold(&worldManifold);
	const b2Body* bodyA = contact->GetFixtureA()->GetBody();
	const b2Body* bodyB = contact->GetFixtureB()->GetBody();
	b2Vec2 point = worldManifold.points[0];
	b2Vec2 vA = bodyA->GetLinearVelocityFromWorldPoint(point);
	b2Vec2 vB = bodyB->GetLinearVelocityFromWorldPoint(point);
	return b2Dot(vB-vA,worldManifold.normal);
}


void InGameState::PreSolve(b2Contact* contact, const b2Manifold* oldManifold)
{

	b2Fixture *a = contact->GetFixtureA(),
		*b=contact->GetFixtureB();
	if(contact->IsTouching()){
		ActorHook *aData=(ActorHook*)a->GetBody()->GetUserData();
		ActorHook *bData=(ActorHook*)b->GetBody()->GetUserData();
		float approachVelocity=calculateApproachVelocity(contact);
		if(isOfTypeUnit(aData->type)){
			Unit *unitA= (Unit*)aData->data;
			if(approachVelocity>8){
				unitA->damageUnit(approachVelocity/2);
				unitA->disturb(5);
			}
		}
		if(isOfTypeUnit(bData->type)){
			Unit *unitB= (Unit*)bData->data;
			if(approachVelocity>8){
				unitB->damageUnit(approachVelocity/2);
				unitB->disturb(5);
			}
		}
		if(aData==NULL||bData==NULL)return;

		if(isOfTypeUnit(aData->type) && isOfTypeUnit(bData->type)){
		/*if(unitA==unitB->p->tower || unitB==unitA->p->tower)
			contact->SetEnabled(false);*/
		}
		if(aData->type==SPELL_TYPE){
			Spell* spell = (Spell*)aData->data;
			spell->onProjectileContact(a->GetBody(),b->GetBody());
		}
		if(bData->type==SPELL_TYPE){
			Spell* spell = (Spell*)bData->data;
			spell->onProjectileContact(b->GetBody(),a->GetBody());
		}

	}
}


void InGameState::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse)
{ 
}

bool ANYUNITSINPOSBOOL=false; 
class  AnyUnitsInPos:public b2QueryCallback{
	bool ReportFixture(b2Fixture* fixture){
		if(fixture->IsSensor())return true;
		ANYUNITSINPOSBOOL=true;
		return false;
	}
};


bool checkTile(b2World *w,b2AABB &box,b2QueryCallback *cb){
	ANYUNITSINPOSBOOL=false;
	w->QueryAABB(cb,box);
	return !ANYUNITSINPOSBOOL;
}


int cheatXI,cheatXF,cheatYI,cheatYF;

void InGameState::render(RenderList *renderList, int vpx, int vpy){
	list<Spell*>::iterator spellIt= spells.begin();
	while(spellIt!=spells.end()){
		(*spellIt)->render(renderList,vpx,vpy);
		spellIt++;
	}
	if(shiftDown)toolbar.getCurrentSpell()->renderPreview(renderList,cheatXI,cheatYI,cheatXF,cheatYF,vpx,vpy);
	/*
	//Draw clusters
	vector<Cluster> clusters=players[1]->myAI.getClusters();
	int selectId= ResourceBankgetResource((string)"selectPixel");
	for(int i=0;i<clusters.size();i++){
		float x = clusters.at(i).x*BOX2D_Scale_Factor_INV-vpx;
		float y = clusters.at(i).y*BOX2D_Scale_Factor_INV-vpy;
		float r = clusters.at(i).radius*BOX2D_Scale_Factor_INV;
		renderList->addRenderItem(selectId,x,y,0,100,0,0,0,r,r);
	}
	//Draw Enemy Bases
	vector<Base> bases=players[1]->myAI.getEnemyBases();
	for(int i=0;i<bases.size();i++){
		float x = bases.at(i).x*BOX2D_Scale_Factor_INV-vpx;
		float y = bases.at(i).y*BOX2D_Scale_Factor_INV-vpy;
		float r = bases.at(i).range*BOX2D_Scale_Factor_INV/2;
		renderList->addRenderItem(selectId,x,y,0,100,0,0,0,r,r);
		vector<Building*> display=bases.at(i).buildings;
		for(int j=0;j<display.size();j++){
			float bx = buildings.at(j)->getWorldX()*BOX2D_Scale_Factor_INV-vpx;
			float by = buildings.at(j)->getWorldY()*BOX2D_Scale_Factor_INV-vpy;
			float br = buildings.at(j)->getBaseRange()*BOX2D_Scale_Factor_INV/2;
			renderList->addRenderItem(selectId,bx,by,0,100,0,0,0,br,br);
		}
	}
	*/
	if(currentBuilding!=NULL){
		int newX=game->getInput()->getVPMouseX(),//-vpx,
			newY=game->getInput()->getVPMouseY();//-vpy;
		newX/=TILE_WIDTH;
		newY/=TILE_HEIGHT;
		newX++;
		newY++;
		newX*=TILE_WIDTH;
		newY*=TILE_HEIGHT;
		bx=newX-currentBuilding->getSpriteType()->getPhysicalW()/2;
		by=newY-currentBuilding->getSpriteType()->getPhysicalH()/2;
		//newX-=currentBuilding->getSpriteType()->getPhysicalW()/2;
		//newY-=currentBuilding->getSpriteType()->getPhysicalH()/2;
		
		AnyUnitsInPos callBack;
		b2AABB rect;
		b2World* world = game->getGSM()->getPhysics()->getWorld();
		mappable=true;
		for(int i=0;i<currentBuilding->w;i++){
			for(int j=0;j<currentBuilding->h;j++){
				int left=newX+i*TILE_WIDTH-currentBuilding->getSpriteType()->getPhysicalW(),
					right=left+TILE_WIDTH,
					top=newY+j*TILE_HEIGHT-currentBuilding->getSpriteType()->getPhysicalH(),
					bottom=top+TILE_HEIGHT;
				rect.lowerBound.x=(float)left+8;
				rect.lowerBound.y=(float)top+8;
				rect.upperBound.x=(float)right-8;
				rect.upperBound.y=(float)bottom-8;
				rect.lowerBound*=BOX2D_Scale_Factor;
				rect.upperBound*=BOX2D_Scale_Factor;
				int id=0;
				if(checkTile(world,rect,&callBack))
					id=ResourceBankgetResource(string("validplacement"));
				else{
					id=ResourceBankgetResource(string("invalidplacement"));
					mappable=false;
				}
				renderList->addRenderItem(id,left-vpx,top-vpy,0,200,0,0);
			}
		}
		currentBuilding->addPreview(bx-vpx,by-vpy,renderList);
	}
	
}


bool InGameState::placeBuilding(Building* b, float x, float y){
	AnyUnitsInPos callBack;
		b2AABB rect;
		b2World* world = game->getGSM()->getPhysics()->getWorld();
		bool mappable=true;
		for(int i=0;i<b->w;i++){
			for(int j=0;j<b->h;j++){
				int left=((int)x)+i*TILE_WIDTH-TILE_WIDTH,
					right=left+TILE_WIDTH,
					top=((int)y)+j*TILE_HEIGHT-TILE_HEIGHT,
					bottom=top+TILE_HEIGHT;
				rect.lowerBound.x=(float)left-32;
				rect.lowerBound.y=(float)top-32;
				rect.upperBound.x=(float)right+32;
				rect.upperBound.y=(float)bottom+32;
				rect.lowerBound*=BOX2D_Scale_Factor;
				rect.upperBound*=BOX2D_Scale_Factor;
				if(!checkTile(world,rect,&callBack))
					return false;
			}
		}
		this->addBuilding(b,(int)(x),(int)y,b->p);
		return true;
}

void InGameState::renderGUI(RenderList *renderList){
	toolbar.render(renderList);
	displayUnits.render(renderList);
	renderList->addRenderItem(goldicon,2,20,0,255,0,0,0,.5f,.5f);
	renderList->addRenderItem(popicon,100,20,0,255,0,0,0,.5f,.5f);
	goldstr.clear();
	popstr.clear();
	wstringstream stream;
	stream<<mainPlayer->gold<<"(+"<<(int)mainPlayer->goldIncome<<")";
	goldstr.append(stream.str());
	stream.str(L"");
	int numUnits= mainPlayer->numCreated-mainPlayer->numLost;
	stream<<numUnits;
	popstr.append(stream.str());
	renderList->addRenderItem(30,35,255,255,255,255,&goldstr,&fontStr,14);
	renderList->addRenderItem(140,35,255,255,255,255,&popstr,&fontStr,14);
	//spDBToolbar.render(renderList);
	
	if(!game->getGSM()->isGameInProgress()){
		int overlayState= game->getGSM()->getOverlayGameState();
		map<int,GUIScreenOverlay*>::iterator find=overlayWorkers.find(overlayState);
		if(find!=overlayWorkers.end())(*find).second->render(renderList);
	}
}


void InGameState::handleMouseInput(int x, int y){
	if(!game->getGSM()->isGameInProgress()){
		int overlayState= game->getGSM()->getOverlayGameState();
		map<int,GUIScreenOverlay*>::iterator find=overlayWorkers.find(overlayState);
		if(find!=overlayWorkers.end())(*find).second->handleMouse(x,y);
		return;
	}
	toolbar.handleMouse(game,x,y);
	displayUnits.handleMouse(this,game,x,y);
	//spDBToolbar.handleMouse( this, game, x, y );
	if(this->shiftDown){
		cheatXI=cheatXF=game->getInput()->getVPMouseX();
		cheatYI=cheatYF=game->getInput()->getVPMouseY();
		
	}
}

//////////////////////////////CLASS - SelectUnitsCallback ////////////////////////

/////////////////// END CLASS ////////////////////////////////

////////////////////////////////////////////
/*
*	handleClickInput - if shift is held, then user is wanting to select units, 
*		and no spell will cast. Otherwise the selected spell, if a single-click 
*		type spell, will cast. 
*/
void InGameState::handleClickInput(int x, int y,int screenX,int screenY){
	if(!game->getGSM()->isGameInProgress()){
		int overlayState= game->getGSM()->getOverlayGameState();
		map<int,GUIScreenOverlay*>::iterator find=overlayWorkers.find(overlayState);
		if(find!=overlayWorkers.end())(*find).second->handleClick(screenX,screenY);
		return;
	}
	Viewport *vp = game->getGUI()->getViewport();
	Cursor *c = game->getGUI()->getCursor(GS_GAME_IN_PROGRESS);
	toolbar.handleClick(game,screenX,screenY);
	displayUnits.handleClick(this,game,x,y);
	//spDBToolbar.handleClick(this, game, screenX, screenY);
	if(!isCursorBusy()){
		vector<Tower*>::iterator beginTower=towers.begin(), endTower=towers.end();
		b2Vec2 scaleMouse(x,y);
		scaleMouse*=BOX2D_Scale_Factor;
		for(;beginTower!=endTower;)
		{
			Tower* t = *beginTower;
			if(!t->isDead()){
				if(t->p==mainPlayer){
					b2AABB rct=t->getAABB();
					if(rct.lowerBound.x<scaleMouse.x && rct.lowerBound.y<scaleMouse.y &&
						rct.upperBound.x>scaleMouse.x && rct.upperBound.y>scaleMouse.y){
							t->mouseClick(game,screenX,screenY);
					}
				}
				beginTower++;
			}else{
				beginTower=towers.erase(beginTower);
				endTower=towers.end();
			}
		}
		vector<Building*>::iterator beginOther=buildings.begin(), endOther=buildings.end();
		for(;beginOther!=endOther;)
		{
			Building* t = *beginOther;
			if(!t->isDead()){
				if(t->p==mainPlayer){
					b2AABB rct=t->getAABB();
					if(rct.lowerBound.x<scaleMouse.x && rct.lowerBound.y<scaleMouse.y &&
						rct.upperBound.x>scaleMouse.x && rct.upperBound.y>scaleMouse.y){
							t->mouseClick(game,screenX,screenY);
					}
				}
				beginOther++;
			}
			else{
				beginOther=buildings.erase(beginOther);
				endOther=buildings.end();
			}
		}
	}
	if( screenY < vp->getViewportHeight()+vp->getViewportOffsetY() ){ 
		if (canCastSpell() && !isCursorBusy()){
			Spell* p = toolbar.getCurrentSpell();
			if(p!=NULL && p->getType()==click){
				//x+=vp->getViewportX()-vp->getViewportOffsetX();
				//y+=vp->getViewportY()-vp->getViewportOffsetY();
				
					if(p->getManaCost(game,x,y,0,0)<=this->mainPlayer->mana){
						mainPlayer->mana-=p->getManaCost(game,x,y,0,0);
						p=p->clone();
						p->start(game,x,y);
						p->owner=mainPlayer;
						spells.push_back(p);
					}
				
			}
			//c->setActiveCursorID(2);

			
		}
		else{
			if(currentBuilding!=NULL&& mappable){
				if(!mainPlayer->getWizard()->buildingsTutorials){
					tutSys->currentTutorial="BuildBarracks";
					game->getGSM()->setOverlayGameState(TUTSYSSCREENOVERLAYNUM);
					mainPlayer->getWizard()->buildingsTutorials=true;
				}
				this->addBuilding(currentBuilding,bx,by,this->players[0]);
				currentBuilding=NULL;
			}
		}
	}
}

void InGameState::handleRightClickInput(int x, int y,int screenX, int screenY){
}

/*
*	handleClickDragInput - if shift is held, then user is wanting to select units, 
*		and no spell will cast.
*/
void InGameState::handleClickDragInput(int iX, int iY, int eX,int eY, int screeniX, int screeniY){
	if(!game->getGSM()->isGameInProgress()){
		int overlayState= game->getGSM()->getOverlayGameState();
		map<int,GUIScreenOverlay*>::iterator find=overlayWorkers.find(overlayState);
		if(find!=overlayWorkers.end())(*find).second->handleClickDrag(screeniX,screeniY,eX,eY);
		return;
	}
	//Select Units.
	Viewport *vp = game->getGUI()->getViewport();
	eX+=vp->getViewportX()-vp->getViewportOffsetX();
	eY+=vp->getViewportY()-vp->getViewportOffsetY();
	cheatXI=iX;
	cheatYI=iY;
	cheatXF=eX;
	cheatYF=eY;

}

/*
*	handleGenericDragInput - if shift is held, then user is wanting to select units, 
*		and no spell will cast.
*/
void InGameState::handleGenericDragInput(int iX, int iY, int eX,int eY,int screeniX,int screeniY){

	Cursor *c = game->getGUI()->getCursor(GS_GAME_IN_PROGRESS);
	if ( shiftDown )
		c->setActiveCursorID(1);
	else
		c->setActiveCursorID(3);

}

/*
*	dispatchClickDragInput
*/
void InGameState::dispatchClickDragInput(int iX, int iY, int eX,int eY,int screeniX,int screeniY){
	if(!game->getGSM()->isGameInProgress()){
		int overlayState= game->getGSM()->getOverlayGameState();
		map<int,GUIScreenOverlay*>::iterator find=overlayWorkers.find(overlayState);
		if(find!=overlayWorkers.end())(*find).second->dispatchClickDrag(screeniX,screeniY,eX,eY);
		return;
	}
	Viewport *vp = game->getGUI()->getViewport();
	
	if( screeniY < vp->getViewportHeight()+vp->getViewportOffsetY() ){
		int gameEX=eX+vp->getViewportX()-vp->getViewportOffsetX();
		int gameEY=eY+vp->getViewportY()-vp->getViewportOffsetY();
		if (canCastSpell() && !isCursorBusy()){
			Spell* p = toolbar.getCurrentSpell();	
			if(p!=NULL && p->getType()==clickdrag){
					if(p->getManaCost(game,iX,iY,gameEX,gameEY)<=this->mainPlayer->mana){
						mainPlayer->mana-=p->getManaCost(game,iX,iY,gameEX,gameEY);
						p=p->clone();
						//iX+=vp->getViewportX()-vp->getViewportOffsetX();
						//iY+=vp->getViewportY()-vp->getViewportOffsetY();
						p->start(game,iX,iY,gameEX,gameEY);
						p->owner=mainPlayer;
						spells.push_back(p);
					}
				
			}
			if(p!=NULL && p->getType()==click){
						mainPlayer->mana-=p->getManaCost(game,iX,iY,gameEX,gameEY);
						//iX+=vp->getViewportX()-vp->getViewportOffsetX();
						//iY+=vp->getViewportY()-vp->getViewportOffsetY();
						p=p->clone();
						p->start(game,iX,iY);
						p->owner=mainPlayer;
						spells.push_back(p);
					}
				
			
		}
		
	}

}
void InGameState::setMainPlayer(Player* p){
	this->mainPlayer=p;
	GameRendererModule::renderer->p=p; 
} 
void InGameState::registerLua(lua_State* s){
		using namespace luabind;
		module(s)[
			class_<InGameState>("InGameState")
				.def("setMe",&setMainPlayer)
				.def("addPlayer",&addPlayer)
				.def("addUnit",&addUnit)
				.def("addTower",&createTower)
		];
	}


	void InGameState::setupMiscDisplayIDs(){ 
		SelectedFX::displayID=ResourceBankgetResource((string)"SelectedFXCircle");
		SelectedFX::textureW=game->getGraphics()->getWorldTextureManager()->getTextureX(SelectedFX::displayID);
		SelectedFX::textureH=game->getGraphics()->getWorldTextureManager()->getTextureY(SelectedFX::displayID);
		Barracks::barOutline= ResourceBankgetResource((string)"ConstructionBarOutline");
		Barracks::barFill= ResourceBankgetResource((string)"ConstructionBarFill");
	}

	void InGameState::addBuilding(Building* u, int x, int y, Player*p){
		//add unit to game
		u->p=p;		
		u->birth(x,y,game);
		game->getGSM()->getSpriteManager()->addBot(u);
		u->birthID=birthCounter++;
		buildings.push_back(u);
	}
class IsBuildingThereCB:public b2QueryCallback{
public:
	Unit** setup;
	b2Vec2 center;
	float min;
	bool ReportFixture(b2Fixture* fixture)
		{
			if(fixture->IsSensor())return true;
			b2Body* body = fixture->GetBody();
			ActorHook* data = (ActorHook*)body->GetUserData();
			if(data->type==BUILDING_TYPE){
				Unit* inRange = (Unit*)data->data;
				float len=(inRange->getBody()->GetPosition()-center).LengthSquared();
				if(len<min){
					*setup=inRange;
				}
			}
			return true;
		}

};

template<class T>
void InGameState::loadUnit(string type,lua_State* s, AnimatedSpriteType *atype){
	luabind::object table = luabind::globals(s)["UnitTypeConstants"][type];
	T::maxSpeedBase=luabind::object_cast<float>(table["maxSpeedBase"]);
	T::attackSpeedBase=luabind::object_cast<float>(table["attackSpeedBase"]);
	T::maxHealthBase=luabind::object_cast<float>(table["maxHealthBase"]);
	T::armorBase=luabind::object_cast<float>(table["armorBase"]);
	T::frictionBase=luabind::object_cast<float>(table["frictionBase"]);
	T::attackRangeBase=luabind::object_cast<float>(table["attackRangeBase"]);
	T::sizeBase=luabind::object_cast<float>(table["sizeBase"]);
	T::aggroRange=luabind::object_cast<float>(table["aggroRange"]);
	T::unitMass=luabind::object_cast<float>(table["unitMass"]);
	T::strengthBase=luabind::object_cast<float>(table["strength"]);
	T::hpScaleX=T::hpScaleY=(float)atype->getTextureWidth() /128;
	T::hpOffX=0;
	T::hpOffY=32*T::hpScaleY;
	T::loadSelf(s);

}
template<class T>
void InGameState::loadBuilding(string type,lua_State* s, AnimatedSpriteType *atype){ 
	luabind::object table = luabind::globals(s)["UnitTypeConstants"][type];
	T::goldCost=luabind::object_cast<float>(table["goldCost"]);
	T::influenceRange=luabind::object_cast<float>(table["influenceRange"]);
	T::constructionPercentBase=luabind::object_cast<float>(table["constructionPercentBase"]);
	T::constructionPercentIncrement=luabind::object_cast<float>(table["constructionPercentIncrement"]);
	T::maxHealthBase=luabind::object_cast<float>(table["maxHealthBase"]);
	T::armorBase=luabind::object_cast<float>(table["armorBase"]);
	T::hpScaleX=T::hpScaleY=(float)atype->getTextureWidth() /128;
	T::hpOffX=0;
	T::hpOffY=32*T::hpScaleY;
	T::loadSelf(s);
}