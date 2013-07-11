#include "stdafx.h"
#include <SourceCode\InGameCode\SpellDB.h>
#include <SourceCode\InGameCode\GUI\SpellButton.h>
#include <SourceCode\InGameCode\Spells\Spell.h>
#include <SourceCode\ProjectBase\ResourceBank.h>

#include <SourceCode\InGameCode\Spells\Fireball.h>
#include <SourceCode\InGameCode\Spells\GravityWell.h>
#include <SourceCode\InGameCode\Spells\GravityBomb.h>
#include <SourceCode\InGameCode\Spells\FlameWall.h>
#include <SourceCode\InGameCode\Spells\Meteor.h>
#include <SourceCode\InGameCode\Spells\MeteorShower.h>
#include <SourceCode\InGameCode\Spells\KineticBooster.h>

//#include "luabind\luabind.hpp"
//#include "lua.h"

#include <map>
#include <utility>

#include "SSSF_SourceCode\game\Game.h"

static int MAX_SPELLS_PER_TYPE = 6;
static int MAX_SPELL_TYPES = 8;
typedef pair<SpellButton*, Spell*> SpellButtonPair;

SpellDB::SpellDB(void)
{
	spellDB = new map<string, SpellButtonPair>();
	spellCount = 0;
	
}

SpellDB::~SpellDB(void)
{
	delete spellDB;
	//***MUST DELETE ALL BUTTONS AND SPELLS FIRST
}

void initializeButton(string spellName, SpellButton *b, luabind::object &spellInfo){
	b->upImg = luabind::object_cast<int>(spellInfo[spellName][1]);
	b->downImg = luabind::object_cast<int>(spellInfo[spellName][2]);
	b->title = spellName;
}

//FAUX FACTORY FUNCTION
Spell* getNewSpell(string type){
	if( type =="Fireball")
		return new Fireball();
	else if( type =="Meteor")
		return new Meteor();
	else if( type =="KineticBooster")
		return new KineticBooster();
	else if( type =="GravityWell")
		return new GravityWell();
	else if( type =="GravityBomb")
		return new GravityBomb();
	else if( type =="MeteorShower")
		return new MeteorShower();
	else if( type =="FlameWall")
		return new FlameWall();
	else
		return NULL;
}


void SpellDB::loadDB(Game *g, lua_State *ls){

	BUTTON_SOCKET = ResourceBankgetResource((string)"SpellButtonSocket");
	//SOME VARIABLES FOR SHARING
	SpellButton *b;
	Spell *sp;
	//luabind::object spellButtonIds;
	//spellButtonIds = luabind::globals(ls)["SPELLBTNSIDS"];

	luabind::object toolbarTable = luabind::globals(ls)["SpellDatabaseDef"];
	for(luabind::iterator i(toolbarTable),end;i!=end;i++){
		b = new SpellButton();
		int j=1;
		b->upImg=ResourceBankgetResource(luabind::object_cast<string>(i->operator[](j++)));//GetUP
		b->downImg=ResourceBankgetResource(luabind::object_cast<string>(i->operator[](j++)));//GetDOWN
		b->title=(luabind::object_cast<string>(i->operator[](j++)));//GETTITLE
		sp = getNewSpell(b->title);
		b->tooltip=(luabind::object_cast<string>(i->operator[](j++)));//GetToolTip
		b->titleFont=(luabind::object_cast<string>(i->operator[](j++)));//GetTitleFont
		b->tooltipFont=(luabind::object_cast<string>(i->operator[](j++)));//GetTitleFont
		b->titleColor[0]=(luabind::object_cast<int>(i->operator[](j++)));
		b->titleColor[1]=(luabind::object_cast<int>(i->operator[](j++)));
		b->titleColor[2]=(luabind::object_cast<int>(i->operator[](j++)));
		b->titleColor[3]=(luabind::object_cast<int>(i->operator[](j++)));
		b->tooltipColor[0]=(luabind::object_cast<int>(i->operator[](j++)));
		b->tooltipColor[1]=(luabind::object_cast<int>(i->operator[](j++)));
		b->tooltipColor[2]=(luabind::object_cast<int>(i->operator[](j++)));
		b->tooltipColor[3]=(luabind::object_cast<int>(i->operator[](j++)));
		b->backgroundID=ResourceBankgetResource(luabind::object_cast<string>(i->operator[](j++)));

		//FINALLY ADD IT TO THE DATABASE
		addSpell(b->title, b, sp);
	}

}

void SpellDB::addSpell(string spellName, SpellButton* spBut, Spell* spell){
	SpellButtonPair sBp = make_pair(spBut, spell);
	(*spellDB)[spellName] = sBp;
	spellCount++;
}

//not currently used
SpellAndButtonPair SpellDB::getSpellButtonPair(string name){
	SpellButtonPair sBp = (*spellDB)[name];
	return sBp;
}

Spell*  SpellDB::getSpell(string name){
	SpellButtonPair sBp = (*spellDB)[name];
	return sBp.second;
}

SpellButton* SpellDB::getSpellButton(string name){
	SpellButtonPair sBp = (*spellDB)[name];
	return sBp.first;
}
//not currently used
void SpellDB::removeSpell(){

}
