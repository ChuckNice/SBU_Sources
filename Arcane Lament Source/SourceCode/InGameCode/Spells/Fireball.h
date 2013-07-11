#pragma once

#include <SourceCode\InGameCode\Spells\Spell.h>
#include <Box2D\Box2D.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <SSSF_SourceCode\gsm\physics\Physics.h>
#include <SSSF_SourceCode\graphics\RenderList.h>
#include <SSSF_SourceCode\gsm\physics\ActorHook.h>
#include <SourceCode\ProjectBase\GroupGameData.h>

#include <SourceCode\InGameCode\Player.h>
class FireballQuery;
class Fireball:public Spell{
	b2Body* self;
public:
	bool explode;
	static int displayId;
	float ang;
	ActorHook hook;
	Fireball(){ 
		type=SpellType::clickdrag;
		spClass=SpellClass::FIRE;
		hook.type=SPELL_TYPE; 
		hook.data=this;
		explode=false;
	}
	virtual Spell* clone(){
		return new Fireball();
	}
	~Fireball(){
	}
	/**
	* Get Mana cost of casting a spell
	*
	**/
	virtual float getManaCost(Game* game, int xi, int yi, int xf, int yf){
		return 10;
	}
	/** 
	Should not be used!
	**/
	virtual bool start(Game* game, int x, int y){return false;}
	/**
		start method for click drag spells
		returns true if spell was sucessfully cast.
		returns false if spell needs to be deleted.
	**/
	virtual bool start(Game* game, int xi, int yi, int xf, int yf){
		Physics* p =game->getGSM()->getPhysics();
		//Construct self
		b2BodyDef selfDefinition;
		selfDefinition.type=b2_dynamicBody; 
		selfDefinition.bullet=true;
		selfDefinition.position.x=(xi)*BOX2D_Scale_Factor;
		selfDefinition.position.y=(yi)*BOX2D_Scale_Factor;
		selfDefinition.userData=&hook;
		self=p->createBody(&selfDefinition);
		b2FixtureDef circle;
		circle.density=.1f;
		b2CircleShape circleShape;
		circleShape.m_radius=12.5f*BOX2D_Scale_Factor;
		circle.shape=&circleShape;
		self->CreateFixture(&circle);
		b2Vec2 toLoc(xf-xi,yf-yi);
		toLoc.Normalize();
		toLoc*=10;
		self->ApplyForceToCenter(toLoc);
		ang=atan2(toLoc.y,toLoc.x)+3.14f/4;
		return true;
	}
	/**
	* Called before the drag spell is about to be cast.
	* 
	**/
	virtual bool preStartDrag(Game* game, int xi, int yi, int xf, int yf){
		return false;
	}
	/**
	* Update method for the spell to update itself and influence the world.
	* Is not called before spell has started.
	**/
	virtual bool update(Game* game);
	/**
	* render method for the spell to render itself onto the world. Is called before spell has started.
	*
	**/
	virtual void render(RenderList *l, int vx, int vy){
		l->addRenderItem(displayId,
			(int)(self->GetPosition().x*BOX2D_Scale_Factor_INV-25.0f)-vx,
				(int)(self->GetPosition().y*BOX2D_Scale_Factor_INV-25.0f)-vy,
				0,
				255,
				64,
				64,
				ang,
				.5f,.5f
				);
	}
	
	virtual void onProjectileContact(b2Body* yours, b2Body* contact);
	void renderPreview(RenderList *l, int xi,int yi, int xf, int yf, int vx, int vy);
};