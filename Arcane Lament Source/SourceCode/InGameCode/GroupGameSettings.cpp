#include <SourceCode\InGameCode\GroupGameSettings.h>
#include <SourceCode\InGameCode\Renderers\InGameRenderer.h>
#include <SourceCode\LuaModules\GameRendererModule.h>
#include <SourceCode\InGameCode\Spells\Fireball.h>
#include <SourceCode\InGameCode\Spells\FlameWall.h>
#include <SourceCode\InGameCode\Spells\Meteor.h>
#include <SourceCode\InGameCode\Spells\MeteorShower.h>
#include <SourceCode\InGameCode\Spells\GravityWell.h>
#include <SourceCode\InGameCode\Spells\GravityBomb.h>
#include <SourceCode\InGameCode\Spells\KineticBooster.h>
#include <SourceCode\InGameCode\Building.h>
#include <SourceCode\InGameCode\SpellDB.h>
#include <SourceCode\WizardSystem\Wizard.h>
#include <SourceCode\InGameCode\New folder\UnitHPBar.h>


bool GameSettings::drawHPBarsTinyUnits=true;
bool GameSettings::drawWarriorAttackEffects=true;
bool GameSettings::drawHPBarsOgre=false;//delete
bool GameSettings::drawOgreAttackEffects=true;

InGameRenderer* GameRendererModule::renderer;
int Fireball::displayId=2;
int KineticBooster::displayId=2;
vector<int> FlameWall::displayIds;
vector<int> GravityWell::displayIds;
vector<int> GravityBomb::displayIds;
int Meteor::displayId=2;
int Meteor::shadowId=2; 
int MeteorShower::displayId=2;
int MeteorShower::shadowId=2;
int SpellDB::BUTTON_SOCKET=4;
int KineticBooster::ALTERNATING_SPEED = 10;
int Building::barFill=1;
int Building::barOutline=1;
int UnitHPBar::container=-1;
int Spell::SpellArrowID,Spell::SpellCircleID,Spell::SpellLineID,Spell::SpellAOEID;
//vector<string> Wizard::DEFAULT_LOADOUT=("Fireball", "GravityWell");
