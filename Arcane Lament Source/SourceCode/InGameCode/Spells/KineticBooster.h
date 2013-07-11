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
#include <math.h>

#include <SourceCode\InGameCode\Player.h>
class KineticBoosterQuery:public b2RayCastCallback{
	public:
	b2Vec2 force;
	Player* p;
	 float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
									  const b2Vec2& normal, float32 fraction)
	{	
		b2Vec2 newForce = force;
		ActorHook *hook=(ActorHook*)fixture->GetBody()->GetUserData();
		if(hook!=NULL && !fixture->IsSensor()){
			if(hook->type==UNIT_TYPE){
				Unit* u = (Unit*)hook->data;
				if(!p->isAlly(u)){
					u->disturb(5);
					u->getBody()->ApplyForceToCenter(newForce);
				}
			}
		}
		return -1.0f;
	}
};


class KineticBooster:public Spell{
	static int ALTERNATING_SPEED;
	b2Vec2 p1,p2,dd,normaldd;
	b2Vec2 sp1,sp2;
	int end;
	KineticBoosterQuery query;
	float texAngle;
	bool alternateTexFlag;
	int alphaCounter;

public:
	static int displayId;
	ActorHook hook;

	KineticBooster(){ 
		alphaCounter = ALTERNATING_SPEED;
		type=SpellType::clickdrag;
		spClass=SpellClass::KINETIC;
		hook.type=SPELL_TYPE; 
		hook.data=this;
		end=300;
		alternateTexFlag = false;
	}
	virtual Spell* clone(){
		return new KineticBooster();
	}
	~KineticBooster(){
	}
	/** 
	Should not be used!
	**/

	virtual float getManaCost(Game* game, int xi, int yi, int xf, int yf){
		return 10;
	}

	virtual bool start(Game* game, int x, int y){
		return false;
	}
	/**
		start method for click drag spells
		returns true if spell was sucessfully cast.
		returns false if spell needs to be deleted.
	**/
	virtual bool start(Game* game, int xi, int yi, int xf, int yf){
		
		p1.x=(float)xi;
		p1.y=(float)yi;
		p2.x=(float)xf;
		p2.y=(float)yf;
		dd=p2-p1;
		normaldd=dd;
		normaldd.Normalize();
		query.force=normaldd;
		query.force*=15;
		sp1=p1;
		sp2=p2;
		sp1*=BOX2D_Scale_Factor;
		sp2*=BOX2D_Scale_Factor;
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
	virtual bool update(Game* game){
		if(end--<0){
			return true;
		}
		query.p=owner;
		b2World* world=game->getGSM()->getPhysics()->getWorld();
		world->RayCast(&query,sp1,sp2);
		return false;
	}
	/**
	* render method for the spell to render itself onto the world. Is called before spell has started.
	*
	**/
	virtual void render(RenderList *l, int vx, int vy){
		int numCast=dd.Length()/30;
		float factor = dd.Length()/numCast;
		b2Vec2 pos;
		int alpha;
		int alpha0 = 50;
		int alpha180 = 180;

		if (--alphaCounter < 0){
			alternateTexFlag = !alternateTexFlag;
			alphaCounter = ALTERNATING_SPEED;
		}


		for(int i=0;i<numCast;i++){

			if(i%2 == 0 ){
				if(!alternateTexFlag)
					alpha = alpha0;
				else
					alpha = alpha180;
			}else{ 
				if (!alternateTexFlag)
					alpha = alpha180;
				else
					alpha = alpha0;
			}

			pos=p1+factor*i*normaldd;
			l->addRenderItem(displayId,
			(int)(pos.x-45-vx),
				(int)(pos.y-20-vy),
				0,
				alpha,
				64,
				64,
				atan2f(normaldd.y, normaldd.x),
				0.35,
				0.35
				);
		}
		
	}
	
	virtual void onProjectileContact(b2Body* yours, b2Body* contact){
	}
	void renderPreview(RenderList *l, int xi,int yi, int xf, int yf, int vx, int vy){
		l->addRenderItem(Spell::SpellCircleID,xi-vx-16,yi-vy-16,0,255,0,0);
		float dx = xf-xi, dy=yf-yi;
		float r = atan2((float)dy,dx);
		float dist = sqrt(dx*dx+dy*dy);
		l->addRenderItem(Spell::SpellLineID,xi-vx,yi-vy,0,255,0,0,r,dist/15,1,1);
		l->addRenderItem(Spell::SpellArrowID,xf-vx-8,yf-vy-8,0,255,0,0,r+3.14/4);
	}
};