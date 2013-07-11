#pragma once

#include <SourceCode\InGameCode\Spells\Spell.h>
#include <Box2D\Box2D.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <SSSF_SourceCode\gsm\physics\Physics.h>
#include <SSSF_SourceCode\graphics\RenderList.h>
#include <SSSF_SourceCode\gsm\physics\ActorHook.h>
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SourceCode\InGameCode\Unit.h>
#include <SourceCode\InGameCode\FX\GenericFXParticles.h>

#include <SourceCode\InGameCode\Player.h>
class MeteorQuery:public b2QueryCallback{
	public:
		Player* p;
		int x,y;
    bool ReportFixture(b2Fixture* fixture)
    {
        b2Body* body = fixture->GetBody();
		if(fixture->IsSensor())return true;
		ActorHook* data = (ActorHook*)body->GetUserData();
		float x= this->x*BOX2D_Scale_Factor,y=this->y*BOX2D_Scale_Factor;
		if(data->type==UNIT_TYPE){
			Unit* u = (Unit*)data->data; 
			if(!p->isAlly(u)){
				u->damageUnit(50);
				u->disturb(10);
				b2Vec2 d(body->GetPosition().x-x,body->GetPosition().y-y);
				d.Normalize();
				d*=150;
				body->ApplyForceToCenter(d);
			}
		}
        return true;
    }
};
class Meteor:public Spell{
	b2Body* self;
public:
	int x, y;
	int explode;
	float area;
	static int displayId;
	static int shadowId;
	float S,dS;
	ActorHook hook;
	Meteor(){ 
		type=SpellType::click;
		spClass=SpellClass::CELESTIAL;
		hook.type=SPELL_TYPE; 
		hook.data=this;
		area=300;
	}
	virtual Spell* clone(){
		return new Meteor();
	}
	~Meteor(){
	}
	/** 
	Should not be used!
	**/
	virtual bool start(Game* game, int x, int y){
		explode=150;
		area=300;
		S=.1f;
		dS=(area/50-S)/explode;
		this->x=x;
		this->y=y;
		return false;
	}

	virtual float getManaCost(Game* game, int xi, int yi, int xf, int yf){
		return 10;
	}

	/**
		start method for click drag spells
		returns true if spell was sucessfully cast.
		returns false if spell needs to be deleted.
	**/
	virtual bool start(Game* game, int xi, int yi, int xf, int yf){
		return false;
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
	virtual bool update(Game* game){
		S+=dS;
		explode--;
		if(explode<0){
			b2World* world=game->getGSM()->getPhysics()->getWorld();
			MeteorQuery q;
			q.x=x;q.y=y;
			b2AABB box;
			q.p=owner;
			box.lowerBound= b2Vec2((x-area/2)*BOX2D_Scale_Factor,(y-area/2)*BOX2D_Scale_Factor);
			box.upperBound= b2Vec2((x+area/2)*BOX2D_Scale_Factor,(y+area/2)*BOX2D_Scale_Factor);
			world->QueryAABB(&q,box);
			FXParticle* p =game->getGSM()->getFXSys()->getParticle(EXPLOSION);
			p->x=(float)x;
			p->y=(float)y;
			game->getGSM()->getWorld()->addParticle(p);
			return true;
		}
		return false;
	}
	/**
	* render method for the spell to render itself onto the world. Is called before spell has started.
	*
	**/
	virtual void render(RenderList *l, int vx, int vy){
		
		l->addRenderItem(shadowId,
			(int)(x)-vx-25,
				(int)(y)-vy-25,
				0,
				200,
				64,
				64,
				0,
				S,S
				);
		if(explode<100){
		l->addRenderItem(displayId,
			(int)(x+explode*10)-vx,
				(int)(y-explode*40)-vy,
				0,
				200,
				64,
				64,
				0,
				S,S
				);
		}
	}
	
	virtual void onProjectileContact(b2Body* yours, b2Body* contact){
	}

	void renderPreview(RenderList *l, int xi,int yi, int xf, int yf, int vx, int vy){
		l->addRenderItem(Spell::SpellAOEID,xi-vx-128,yi-vy-128,0,255,0,0,0,area/256,area/256);
	}
};