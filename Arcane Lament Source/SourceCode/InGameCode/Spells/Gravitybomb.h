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

#include <SourceCode\InGameCode\Player.h>
class GravityBomb:public Spell{
	b2Body* self;
	b2Fixture* selfCircle;
	SensorSpecificFunctor<GravityBomb> inRange;
	list<Unit*> unitsCaught;
	int count;
	unsigned int currentFrame;
public:
	int explode;
	static vector<int> displayIds;
	ActorHook hook;
	GravityBomb():inRange(this,&GravityBomb::addUnit){ 
		type=SpellType::click;
		spClass=SpellClass::GRAVITY;
		hook.type=SPELL_TYPE; 
		hook.data=this;
		explode=100;
		count = 0;
		currentFrame = 0;
		unitsCaught.empty();
		inRange.setLeave(&GravityBomb::doNothing);
	}
	virtual Spell* clone(){
		return new GravityBomb();
	}
	~GravityBomb(){
	}
	virtual void addUnit(Unit* u){
		if(!owner->isAlly(u))unitsCaught.push_back(u);
	}
	virtual void doNothing(Unit* u){
		unitsCaught.remove(u);
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
		circleShape.m_radius=15;
		circle.shape=&circleShape;
		selfCircle=self->CreateFixture(&circle);
		return true;
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
				b2Vec2 toCenter(rand()%50-25+u->getBody()->GetPosition().x-self->GetPosition().x,rand()%50-25+u->getBody()->GetPosition().y-self->GetPosition().y);
				toCenter.Normalize();
				toCenter*=200;
				u->disturb(5);
				u->damageUnit(10.0f);
				u->getBody()->ApplyForceToCenter(toCenter);
			
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
			u->damageUnit(.005f);
			u->disturb(0);
		}
		return false;
	}
	/**
	* render method for the spell to render itself onto the world. Is called before spell has started.
	*
	**/
	virtual void render(RenderList *l, int vx, int vy){

		if(++count > 4){
			currentFrame++;
			if(currentFrame >= displayIds.size())
				currentFrame = 0;
			count = 0;
		}

		l->addRenderItem(displayIds[currentFrame],
			(int)(self->GetPosition().x*BOX2D_Scale_Factor_INV)-vx-25,
				(int)(self->GetPosition().y*BOX2D_Scale_Factor_INV)-vy-25,
				0,
				180,
				64,
				64,
				self->GetAngle(),
				2.0,2.0);
	}
	
	virtual void onProjectileContact(b2Body* yours, b2Body* contact){
	}
	void renderPreview(RenderList *l, int xi,int yi, int xf, int yf, int vx, int vy){
		l->addRenderItem(Spell::SpellAOEID,xi-vx-128,yi-vy-128,0,255,0,0,0,30*BOX2D_Scale_Factor_INV/256,30*BOX2D_Scale_Factor_INV/256);
	}
};