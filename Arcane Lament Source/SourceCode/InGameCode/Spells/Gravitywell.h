#pragma once

#include <SourceCode\InGameCode\Spells\Spell.h>
#include <Box2D\Box2D.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <SSSF_SourceCode\gsm\physics\Physics.h>
#include <SSSF_SourceCode\graphics\RenderList.h>
#include <SSSF_SourceCode\gsm\physics\ActorHook.h>
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SourceCode\InGameCode\SensorFunctor.h>
#include <SourceCode\InGameCode\Unit.h>
#include <time.h>

#include <SourceCode\InGameCode\Player.h>
static int FRAME_DISPLAY_TIME = 30;
static int NUM_ANIM_CYCLES_PER_CLICK = 5;

class GravityWell:public Spell{
	b2Body* self;
	b2Fixture* selfCircle;
	SensorSpecificFunctor<GravityWell> inRange;
	list<Unit*> unitsCaught;
	int count;
	int currentFrame;
	float rotAngle;
	int numVortexToDispl;

public:
	int explode;
	static vector<int> displayIds;
	ActorHook hook;

	GravityWell():inRange(this,&GravityWell::addUnit){ 
		type=SpellType::click;
		spClass=SpellClass::GRAVITY;
		hook.type=SPELL_TYPE; 
		hook.data=this;
		explode=200;
		count = FRAME_DISPLAY_TIME;//countdown til frame change
		currentFrame = 0;
		numVortexToDispl = NUM_ANIM_CYCLES_PER_CLICK;
		rotAngle = 0.0;
		unitsCaught.empty();
		inRange.setLeave(&GravityWell::doNothing);
	}
	virtual Spell* clone(){
		return new GravityWell();
	}
	~GravityWell(){
	}
	virtual void addUnit(Unit* u){
		if(!owner->isAlly(u))unitsCaught.push_back(u);
	}
	virtual void doNothing(Unit* u){
		unitsCaught.remove(u);
	}

	virtual float getManaCost(Game* game, int xi, int yi, int xf, int yf){
		return 10;
	}
	/** 
	Should not be used!
	**/
	virtual bool start(Game* game, int x, int y){
		Physics* p =game->getGSM()->getPhysics();
		//Construct self
		b2BodyDef selfDefinition;
		selfDefinition.position.x=(x)*BOX2D_Scale_Factor;
		selfDefinition.position.y=(y)*BOX2D_Scale_Factor;
		selfDefinition.userData=&hook;
		self=p->createBody(&selfDefinition);
		b2FixtureDef circle;
		circle.isSensor=true;
		circle.userData=&inRange;
		b2CircleShape circleShape;
		circleShape.m_radius=10;
		circle.shape=&circleShape;
		selfCircle=self->CreateFixture(&circle);
		
		return true;
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

		if(explode--<0){
			list<Unit*>::iterator unitsIt=unitsCaught.begin();
			while(unitsIt!=unitsCaught.end()){
				Unit* u = *unitsIt;
				if(u->isDead())
				{
					unitsIt=unitsCaught.erase(unitsIt);
					continue;
				}else
					unitsIt++;
				b2Vec2 toCenter(u->getBody()->GetPosition().x-self->GetPosition().x,u->getBody()->GetPosition().y-self->GetPosition().y);
				toCenter.Normalize();
				toCenter*=50;
				u->disturb(5);
				u->getBody()->ApplyForceToCenter(toCenter);
				u->damageUnit(10.0f);

			}
			game->getGSM()->getPhysics()->getWorld()->DestroyBody(self);
			return true;
		}
		list<Unit*>::iterator unitsIt=unitsCaught.begin();
		while(unitsIt!=unitsCaught.end()){
			Unit* u = *unitsIt;
			if(u->isDead())
			{
				unitsIt=unitsCaught.erase(unitsIt);
				continue;
			}else
				unitsIt++;

			b2Vec2 toCenter(u->getBody()->GetPosition().x-self->GetPosition().x,u->getBody()->GetPosition().y-self->GetPosition().y);
			toCenter.Normalize();
			toCenter*=-10;
			//u->disturb(5);
			u->getBody()->ApplyForceToCenter(toCenter);

		}

		if(count-- < 0){
			currentFrame++;
			if( currentFrame >= displayIds.size()) //loop keyframes
				currentFrame = 0;
			count = FRAME_DISPLAY_TIME;	//reset timer
		}

		rotAngle += 0.1;
		if ( rotAngle >= 6.28)
			rotAngle = 0.0;	

		selfCircle->GetShape()->m_radius*=.9999f;
		return false;
	}
	/**
	* render method for the spell to render itself onto the world. Is called before spell has started.
	*
	**/
	virtual void render(RenderList *l, int vx, int vy){
		//UPDATE ANIMATION KEY
		//time_t timer = time(NULL);
		float scaleFactor = selfCircle->GetShape()->m_radius*0.5 * (abs(sinf(3.14/FRAME_DISPLAY_TIME*count)*.3+.7f));
		
		for(;numVortexToDispl>=1; numVortexToDispl--){
			l->addRenderItem(displayIds[currentFrame],
				(int)(self->GetPosition().x*BOX2D_Scale_Factor_INV)-vx-25,
				(int)(self->GetPosition().y*BOX2D_Scale_Factor_INV)-vy-25,
				0,
				70,	//ALPHA
				64,
				64,
				rotAngle+5.324f*numVortexToDispl,
				scaleFactor,
				scaleFactor
				);
		}
		numVortexToDispl = NUM_ANIM_CYCLES_PER_CLICK;
	}

	virtual void onProjectileContact(b2Body* yours, b2Body* contact){
		explode=true;
	}
	void renderPreview(RenderList *l, int xi,int yi, int xf, int yf, int vx, int vy){
		l->addRenderItem(Spell::SpellAOEID,xi-vx-128,yi-vy-128,0,255,0,0,0,20*BOX2D_Scale_Factor_INV/256,20*BOX2D_Scale_Factor_INV/256);
	}
};