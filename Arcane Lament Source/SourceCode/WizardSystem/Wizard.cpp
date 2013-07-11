#include <stdafx.h>
#include <SourceCode\WizardSystem\Wizard.h>
#include <SourceCode\InGameCode\GUI\SpellButton.h>
#include <SourceCode\InGameCode\InGameState.h>


// Ability to increase the spell level
void Wizard::levelUpSpell(string spellToLevelUp){	
	map<string, int>::iterator it = unlockedSpells.begin();
	while( it != unlockedSpells.end() ){
		if(it->first.compare(spellToLevelUp)==0){
			it->second++;
			break;
		}
	}
}

void Wizard::initializeToolbarOrder(void){
	for ( int p = 0; p != DEFAULT_LOADOUT.size(); p++ ){
		toolbarOrder.insert(make_pair( p, DEFAULT_LOADOUT[p]));
	}
}

void Wizard::initializeUnlockedSpells(void){
	for ( int p = 0; p != DEFAULT_LOADOUT.size(); p++ ){
		unlockedSpells.insert(make_pair(DEFAULT_LOADOUT[p], 1));
	}
}