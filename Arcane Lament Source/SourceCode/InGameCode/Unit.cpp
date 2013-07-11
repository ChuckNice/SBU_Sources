#include "stdafx.h"
#include <SourceCode\InGameCode\Unit.h>
#include <SourceCode\InGameCode\Player.h>
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SourceCode\InGameCode\StatusFX.h>
#include <SourceCode\InGameCode\StatusFX\SelectedFX.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <SourceCode\InGameCode\InGameState.h>
#include <SourceCode\InGameCode\FX\GenericFXParticles.h>
#include <SourceCode\InGameCode\Units\Archer.h>
#include <SourceCode\InGameCode\Units\WizardUnit.h>
#include <SourceCode\InGameCode\Units\Ogre.h>
#include <SourceCode\InGameCode\Units\Knight.h>


bool Unit::cyclicSelect=true;

void Unit::render(RenderList *list,Viewport *vp){
		if(!isVisible())return;
		float x=(body->GetPosition().x*BOX2D_Scale_Factor_INV),y=(body->GetPosition().y*BOX2D_Scale_Factor_INV);
		float l= x+getSpriteType()->getOffsetX(),
			  t= y+getSpriteType()->getOffsetY();
		if(vp->areWorldCoordinatesInViewport(l,t,getSpriteType()->getTextureWidth(),getSpriteType()->getTextureHeight())){//spriteType->getTextureWidth(),spriteType->getTextureHeight()))
			x-=vp->getViewportX();
			y-=vp->getViewportY();
			l-=vp->getViewportX();
			t-=vp->getViewportY();
			if(selected && Unit::cyclicSelect)
				list->addRenderItem(SelectedFX::displayID,x-SelectedFX::textureW/2.0f,y-SelectedFX::textureH*.3f,0,255,0,0,0.0f,
					selectedScaleX,selectedScaleY);
			list->addRenderItem(this->getCurrentImageID(),
				(int)l,
				(int)t,
				0,
				getAlpha(),
				64,
				64,
				getRotTransform()
				);
			if(p!=NULL && !isDead()){
				list->addRenderItem(p->flagID,
					(int)l,
					(int)t,
					0,
					getAlpha(),
					64,
					64,
					getRotTransform()
					);
				if(this->displayHPBar()){
					list->addRenderItem(p->fillID,
						(int)l-this->getHPOffX(),
						(int)t-this->getHPOffY(),
						0,
						getAlpha(),
						64,
						64,
						getRotTransform(),
						this->getHPScaleX()*hp/getMaxHealth(),
						this->getHPScaleY(),
						1
						);
					list->addRenderItem(UnitHPBar::container,
						(int)l-this->getHPOffX(),
						(int)t-this->getHPOffY(),
						0,
						getAlpha(),
						64,
						64,
						getRotTransform(),
						this->getHPScaleX(),
						this->getHPScaleY(),
						1
						);
				}
			}
			renderStatusFX(list,x,y);
		}
	}


void Unit::renderStatusFX(RenderList* rList, int xOff,int yOff)
{
	
	list<StatusFX*>::iterator start=statusFXList.begin(),
		end=statusFXList.end();
	for(;start!=end;start++){
		StatusFX* fx= *start;
		fx->render(rList,xOff,yOff);
		if(fx->isDestroyed())
		{
			fx->release();
			start=statusFXList.erase(start);
		}
	}
}
void Unit::addStatusFX(StatusFX* fx){
	fx->applyToUnit(this);
	statusFXList.push_back(fx);
}

void Unit::generalBirth(){
	issueStopOrder();
	targetUnit=NULL;
	this->lastWaypointGeneration=-1;
	selected=false;
	selectedScaleX=getTextureWidth()/SelectedFX::textureW;
	selectedScaleY=getTextureHeight()/SelectedFX::textureH;
	maxSpeedFactor=attackSpeedFactor=maxHealthFactor=armorFactor=strengthFactor=1.0f;
	maxHealthIncrease=armorIncrease=strengthIncrease=0;
	p->armyStrength+=this->strengthEstimate();
	p->numCreated++;
	waypoints.clear();
	isDisturbed=0;
	if(typeid(*this)==typeid(Warrior))
		p->numWarriorsCreated++;
	if(typeid(*this)==typeid(WizardUnit))
		p->numWizardsCreated++;
	if(typeid(*this)==typeid(Ogre))
		p->numOgresCreated++;
	if(typeid(*this)==typeid(Archer))
		p->numArchersCreated++;
	if(typeid(*this)==typeid(Knight))
		p->numKnightsCreated++;
}
void Unit::generalRelease(){
	if(p!=NULL){
		p->numLost++;
		if(typeid(*this)==typeid(Warrior))
			p->numWarriorsLost++;
		if(typeid(*this)==typeid(WizardUnit))
			p->numWizardsLost++;
		if(typeid(*this)==typeid(Ogre))
			p->numOgresLost++;
		if(typeid(*this)==typeid(Archer))
			p->numArchersLost++;
		if(typeid(*this)==typeid(Knight))
			p->numKnightsLost++;
		

		list<StatusFX*>::iterator start=statusFXList.begin(),
		end=statusFXList.end();
		for(;start!=end;start++){
			StatusFX* fx= *start;
			fx->release();
		}
		statusFXList.clear();
		p->armyStrength-=this->strengthEstimate();
		p=NULL;
	}
}

class AlertUnitsCallback:public b2QueryCallback{
public:
	Unit *attackingUnit;
	Unit * me;
	bool ReportFixture(b2Fixture* fixture)
	{
		if(fixture->IsSensor())return true;
		b2Body* body = fixture->GetBody();
		ActorHook* data = (ActorHook*)body->GetUserData();
		if(data->type==UNIT_TYPE){
			Unit* inRange = (Unit*)data->data;
			if(inRange->p->isAlly(me) && inRange->targetUnit==NULL){
				inRange->targetThisUnit(attackingUnit);
			}
		}
		return true;
	}
};


void Unit::attackUnit(Game* g,float damage,Unit* u){
	damageUnit(damage);
	if(alertCooldown<=0){
		b2World* world=g->getGSM()->getPhysics()->getWorld();
		b2AABB alertRct;
		float x=getWorldX(),y=getWorldY();
		alertRct.lowerBound.x=x-20;
		alertRct.lowerBound.y=y-20;
		alertRct.upperBound.x=x+20;
		alertRct.upperBound.y=y+20;
		AlertUnitsCallback cb;
		cb.attackingUnit=u;
		cb.me=this;
		world->QueryAABB(&cb,alertRct);
		alertCooldown=100;
	}
	FXParticleSprite *bloodpool=(FXParticleSprite*)g->getGSM()->getFXSys()->getParticle(BloodPoolParticleID);
	b2Vec2 vec= u->getBody()->GetPosition()-getBody()->GetPosition();
	bloodpool->x=getX()+(rand()%50-25);
	bloodpool->y=getY()+(rand()%50-25);
	bloodpool->r=rand()/50;
	g->getGSM()->getWorld()->addLowerParticle(bloodpool); 

	FXParticleSprite *blood=(FXParticleSprite*)g->getGSM()->getFXSys()->getParticle(BloodParticleID);
	vec= u->getBody()->GetPosition()-getBody()->GetPosition();
	blood->x=getX();
	blood->y=getY();
	g->getGSM()->getWorld()->addParticle(blood); 
	if(state==standby)
		targetUnit=u;
}



void Unit::checkAlerts(Game* game){
}