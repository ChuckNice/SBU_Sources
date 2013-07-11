#pragma once

//************COPY OF METEOR FOR TESTING PURPOSES SO I MAY ADD THE BUTTON TO THE TOOL BAR*******************
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

struct impact{
		float x, y;
		float s, ds;
		float area;
		int startTime;
		int explodeTimer;
		bool dead;
	};

class MeteorShowerQuery:public b2QueryCallback{
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
				u->disturb(5);
				b2Vec2 d(body->GetPosition().x-x,body->GetPosition().y-y);
				d.Normalize();
				d*=150;
				body->ApplyForceToCenter(d);
			}
		}
        return true;
    }
};

/////////////// CLASS ////////////////
class MeteorShower:public Spell{
	b2Body* self;
public:
	vector<impact> mySpell;
	int x, y;
	int explode;
	float area;
	static int displayId;
	static int shadowId;
	float S,dS;
	ActorHook hook;
	MeteorShower(){ 
		type=SpellType::click;
		spClass=SpellClass::CELESTIAL;
		hook.type=SPELL_TYPE; 
		hook.data=this;
		area=350;
	}
	virtual Spell* clone(){
		return new MeteorShower();
	}
	~MeteorShower(){
	}

	

	/** 
	Start for click spells
	**/
	virtual bool start(Game* game, int x, int y){
		S=.1f;
		dS=	0.01f;			//(area/50-S)/explode;
		this->x=x;
		this->y=y;
		area=350;
			srand(time(NULL));
		for(int i=0; i<6; i++){
			int neg = (rand()%2 == 0) ? -1 : 1;		
			explode=150+(rand()%50)*neg;
			area=100+(rand()%100)*neg;

			impact imp;

			imp.explodeTimer = explode;
			imp.area = area;
			imp.s = S;
			imp.ds = dS;
			imp.x = this->x+(rand()%100)*neg;
			neg = (rand()%2 == 0) ? -1 : 1;
			imp.y = this->y+(rand()%100)*neg;
			imp.startTime = rand()%90;
			imp.dead = false;

			mySpell.push_back(imp);
		}
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
		for(int i = 0; i<6; i++){
			impact *imp = &(mySpell.at(i));
			imp->s += imp->ds;
			imp->explodeTimer--;

			if(imp->explodeTimer<0 && !mySpell[i].dead){

				b2World* world=game->getGSM()->getPhysics()->getWorld();
				MeteorShowerQuery q;
				q.x=x;q.y=y;
				b2AABB box;
				q.p=owner;
				box.lowerBound= b2Vec2((imp->x-area/2)*BOX2D_Scale_Factor,(imp->y-area/2)*BOX2D_Scale_Factor);
				box.upperBound= b2Vec2((imp->x+area/2)*BOX2D_Scale_Factor,(imp->y+area/2)*BOX2D_Scale_Factor);
				world->QueryAABB(&q,box);
				FXParticle* p =game->getGSM()->getFXSys()->getParticle(EXPLOSION);
				p->x=(float)imp->x;
				p->y=(float)imp->y;
				p->sX=0.1f;
				p->sY=0.1f;
				game->getGSM()->getWorld()->addParticle(p);
				mySpell.at(i).dead = true;
				
				vector<impact>::iterator it = mySpell.begin();
				for(;it != mySpell.end(); it++){
					if(it->dead)
						continue;
					return false;
				}
				return true;
			}
		}
		return false;
	}

	/**
	* render method for the spell to render itself onto the world. Is called before spell has started.
	*
	**/
	virtual void render(RenderList *l, int vx, int vy){

		for(int i = 0; i<6; i++){

			impact *imp = &(mySpell.at(i));
			if(!imp->dead && imp->startTime--<0){
				l->addRenderItem(shadowId,
					(int)(imp->x)-vx-25,
					(int)(imp->y)-vy-25,
					0,
					200,
					64,
					64,
					0,
					imp->s,imp->s
					);
				if(imp->explodeTimer<100){
					l->addRenderItem(displayId,
						(int)(imp->x+imp->explodeTimer*25)-vx,
						(int)(imp->y-imp->explodeTimer*40)-vy,
						0,
						200,
						64,
						64,
						0,
						imp->s,imp->s
						);
				}
			}
		}
	}

	virtual void onProjectileContact(b2Body* yours, b2Body* contact){
	}
	void renderPreview(RenderList *l, int xi,int yi, int xf, int yf, int vx, int vy){
		l->addRenderItem(Spell::SpellAOEID,xi-vx-128,yi-vy-128,0,255,0,0,0,area/256,area/256);
	}
};