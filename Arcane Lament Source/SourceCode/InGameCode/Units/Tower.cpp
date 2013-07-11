#include <SourceCode\InGameCode\Units\Tower.h>
#include <SourceCode\InGameCode\AIModule\SimpleAI.h>
float Tower::influenceRangeBase=100;
float Tower::maxHealthBase=50;
float Tower::armorBase=10;


float Tower::hpScaleX,Tower::hpScaleY;
float Tower::hpOffX,Tower::hpOffY;

void Tower::mouseClick(Game* g, int x, int y){
}

void Tower::loadSelf(lua_State* s){
	luabind::object table = luabind::globals(s)["UnitTypeConstants"]["Tower"];
	maxHealthBase=luabind::object_cast<float>(table["maxHealthBase"]);
	armorBase=luabind::object_cast<float>(table["armorBase"]);
	influenceRangeBase=luabind::object_cast<float>(table["influenceRange"]);
}

void Tower::attackUnit(Game* g,float damage,Unit* u){
	damageUnit(damage);
	if(state==standby)
		targetUnit=u;
}