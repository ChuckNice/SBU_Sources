#pragma once
#include <SSSF_SourceCode\graphics\RenderList.h>
#include <SSSF_SourceCode\game\Game.h>


enum SpellType{
	click,
	clickdrag
};
enum SpellClass{
	FIRE,
	GRAVITY,
	KINETIC,
	WATER,
	EARTH,
	CELESTIAL,
	LIGHTNING,
	BLACK
};

class Player;
class b2Body;
class Spell{
protected:
	/**
		Spell type which determines which start method to use.
	**/
	SpellType type;
	SpellClass spClass;
public:
	Player* owner;
	static int SpellArrowID,SpellCircleID,SpellLineID,SpellAOEID;
	SpellClass getSpellClass()		{	return spClass;	}
	SpellType getType()				{	return type;	}

	/**
		start method for click spells
		returns true if spell was sucessfully cast.
		returns false if spell needs to be deleted.
	**/
	virtual bool start(Game* game, int x, int y)=0;


	/**
		start method for click drag spells
		returns true if spell was sucessfully cast.
		returns false if spell needs to be deleted.
	**/
	virtual bool start(Game* game, int xi, int yi, int xf, int yf)=0;


	/**
	* Called before the drag spell is about to be cast.
	* 
	**/
	virtual bool preStartDrag(Game* game, int xi, int yi, int xf, int yf)=0;


	/**
	* Get Mana cost of casting a spell
	*
	**/
	virtual float getManaCost(Game* game, int xi, int yi, int xf, int yf)=0;


	/**
	* Update method for the spell to update itself and influence the world.
	* Is not called before spell has started.
	**/
	virtual bool update(Game* game)=0;


	/**
	* render method for the spell to render itself onto the world. Is called before spell has started.
	*
	**/
	virtual void render(RenderList *l, int vx, int vy)=0;


	virtual Spell* clone()=0;
	virtual void onProjectileContact(b2Body* yours, b2Body* contact)=0;
	virtual void renderPreview(RenderList *l, int xi,int yi, int xf, int yf, int vx, int vy){}
};


