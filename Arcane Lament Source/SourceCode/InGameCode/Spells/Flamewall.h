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

#include <SourceCode\InGameCode\Player.h>
class FlameWallQuery:public b2RayCastCallback{

	public:
		Player* p;
     float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
                                      const b2Vec2& normal, float32 fraction)

    {
		if(fixture->IsSensor())return -1.0f;
		ActorHook *hook=(ActorHook*)fixture->GetBody()->GetUserData();
		if(hook!=NULL){
			if(hook->type==UNIT_TYPE){
				Unit* u = (Unit*)hook->data;
				if(!p->isAlly(u))u->damageUnit(.5f);	
			}
		}
		return -1.0f;
    }
};



class FlameWall:public Spell{
	b2Vec2 p1,p2,dd;
	b2Vec2 sp1,sp2;
	int end;
	int count;
	int r;
	float nearEnd,nearBegin, dNB,dNE;
	FlameWallQuery query;
	int spacing;
	int windup,winddown;
public:
	static vector<int> displayIds;
	ActorHook hook;
	FlameWall(){ 
		type=SpellType::clickdrag;
		spClass=SpellClass::FIRE;
		hook.type=SPELL_TYPE; 
		hook.data=this;
		end=300;
		count=r=0;
		nearBegin=nearEnd=0;
		spacing=20;
		windup=20;
		winddown=20;
	}
	virtual Spell* clone(){
		return new FlameWall();
	}
	~FlameWall(){
	}
	/** 
	Should not be used!
	**/
	virtual bool start(Game* game, int x, int y){
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
		
		p1.x=(float)xi;
		p1.y=(float)yi;
		p2.x=(float)xf;
		p2.y=(float)yf;
		dd=p2-p1;
		dNB= dd.Length()/spacing/windup;
		dNE=dd.Length()/spacing/winddown;
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
		b2Vec2 normalDD= dd;
		normalDD.Normalize();
		//Animation duration
		if(++count>4){
			r=rand()%displayIds.size();
			count=0;
		}
		nearBegin+=dNB;
		int init=0;
		if(end<50)
			nearEnd+=dNE;
		for(int i=(end>50)?0:(int)nearEnd;i<numCast&& i<nearBegin;i++){
			pos=p1+factor*i*normalDD;
			l->addRenderItem(displayIds[(i+r)%displayIds.size()],
			(int)(pos.x-25-vx),
				(int)(pos.y-25-vy),
				0,
				180,
				64,
				64
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