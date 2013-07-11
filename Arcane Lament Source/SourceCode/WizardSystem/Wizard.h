#pragma once
#include <stdafx.h>
//#include <SourceCode\InGameCode\GUI\SpellToolbar.h>
//#include <SourceCode\InGameCode\InGameState.h>
//#include <SourceCode\InGameCode\SpellDB.h>
//#include <SourceCode\InGameCode\InGameState.h>

static int MAX_TOOLBARS = 1;
static int AVAILABLE_SLOTS = 4;
//static const char* DEFAULT_LOADOUT[] = {"Fireball", "GravityWell"};

//class Game;
//class InGameState;
class SpellButton;
class Wizard{
	int level;
	int experiencePts;	//Exp points in general
	int spellPts;		//Points for making skills
	int spellPowerPts;	//Points for leveling up ability
	vector<string> DEFAULT_LOADOUT;


	//PLAYER MAY START WITH ONE SMALL SPELL TOOLBAR AND GAIN SLoTS AS LEVEL INCREASES
	map<string, int>		unlockedSpells;	//first string is spell id, 2nd is spell level
	multimap<int, string>	toolbarOrder;	//first int in toolbar 0-2, second is spell at pos 0-5. 
	
public:
	bool startGameTutorial, buildingsTutorials;
	Wizard(){
		startGameTutorial=buildingsTutorials=false;
		level=1;
		experiencePts=0;
		spellPowerPts=0;
		spellPts=0;
		DEFAULT_LOADOUT.push_back("Fireball");
		DEFAULT_LOADOUT.push_back("Meteor");
		DEFAULT_LOADOUT.push_back("GravityWell");
		DEFAULT_LOADOUT.push_back("GravityBomb");
		DEFAULT_LOADOUT.push_back("MeteorShower");
		DEFAULT_LOADOUT.push_back("KineticBooster");
		DEFAULT_LOADOUT.push_back("FlameWall");

		initializeUnlockedSpells();
		initializeToolbarOrder();
	}
	~Wizard(){}

	map<string, int>&		getUnlockedSpells( void )	{	return unlockedSpells;		}
	multimap<int, string>&	getToolbarOrder( void )		{	return toolbarOrder;		}

	void setLevel( int newLevel )				{	level = newLevel;				}
	void addExperiencePts (	int addExpAmt )		{	experiencePts += addExpAmt;		}
	void subExperiencePts (	int subExpAmt )		{	experiencePts -= subExpAmt;		}

	void addSpellPts (	int addSpellPts )		{	spellPts += addSpellPts;		}
	void subSpellPts (	int subSpellPts )		{	spellPts -= subSpellPts;		}

	void addSpellPowerPts (	int addPowPts )		{	spellPowerPts += addPowPts;		}
	void subSpellPowerPts (	int subPowPts )		{	spellPowerPts -= subPowPts;		}

	int getLevel(void)							{	return level;					}
	int getExperiencePts(void)					{	return experiencePts;			}
	int getMaxToolbars(void)					{	return MAX_TOOLBARS;			}
	int getAvailSlots(void)						{	return AVAILABLE_SLOTS;			}

	// Ability to increase the spell level
	void levelUpSpell(string spellToLevelUp);

	// Ability to reorder the toolbar allowing blank spaces
	void initializeToolbarOrder(void);
	void initializeUnlockedSpells(void);

	//Add switch toolbar rotate with a hotkey in game

};