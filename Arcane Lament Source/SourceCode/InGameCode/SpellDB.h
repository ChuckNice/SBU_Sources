#pragma once
#include "stdafx.h"
#include "SSSF_SourceCode\game\Game.h"

#include <SourceCode\InGameCode\GUI\SpellButton.h>
#include <SourceCode\InGameCode\Spells\Spell.h>
#include <SourceCode\InGameCode\GUI\SpellToolbar.h>
//#include <SourceCode\InGameCode\Spells\SpellFactory.h>

#include <map>
#include <utility>
//#include "luabind\luabind.hpp"
//#include "lua.h"

typedef pair<SpellButton*, Spell*> SpellAndButtonPair;

class SpellFactory;
class SpellDB{
private:
	map<string, SpellAndButtonPair> *spellDB;
	int spellCount;
	lua_State* state;
	SpellFactory *mySpellFactory;

public:
	SpellDB(void);
	~SpellDB(void);

	static int BUTTON_SOCKET;

	void addSpell(string spellName, SpellButton* spBut, Spell* spell);
	void removeSpell();
	void loadDB(Game* g, lua_State *ls);

	SpellAndButtonPair	getSpellButtonPair(string name);
	Spell*				getSpell(string name);
	SpellButton*		getSpellButton(string name);

	map<string, SpellAndButtonPair>* getSpellDB(){
		return spellDB;
	}

	map<string, SpellAndButtonPair>::iterator getDbIterator(int zero_or_one){	
		if(zero_or_one == 1)
			return spellDB->end(); 
		else 
			return spellDB->begin();
	}

	int getSpellCount(){ return spellCount; }

	/*static void registerLua(lua_State* s){
		using namespace luabind;
		module(s)[
			class_<SpellDB>("SpellDB")
				.def("addSpell",&addSpell)
		];
	}*/
};

