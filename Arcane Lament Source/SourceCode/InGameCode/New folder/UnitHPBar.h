#pragma once
#include "SSSF_SourceCode\gsm\fx\FXParticle.h"
#include <SourceCode\InGameCode\Unit.h>
#include <SourceCode\InGameCode\Player.h>
static const wstring UNITHPBARKEY=L"HPBARUNIT";
class UnitHPBar:public FXParticle{
public:
	static int container;
	Unit* u;
	int containerID;
	int width,height;
	int offx, offy;
	UnitHPBar(){
		key=UNITHPBARKEY;
		containerID=0;
		width=128;
		height=32;
	}
	void calcOffsets(Unit* u){
		offx=(int)(-u->getSpriteType()->getOffsetX());//u->getTextureWidth()-u->getWidth()/2.0f;
		offy=(int)(-u->getSpriteType()->getOffsetY()+getH()*sY);
	}
	void setContainer(int image){
		containerID=image;
	}
	virtual unsigned int getCurrentImageID()
	{
		return -1;
	}
	virtual int getW(){
		return 	width;
	}
	
	virtual int getH(){
		return height;
	}
	virtual void render(RenderList* list,int vX,int vY){
		if(u==NULL)return;
		
		list->addRenderItem(	u->p->fillID,
											((int)x-vX),
											((int)y-vY),
											0,
											(int)a,
											getW(),
											getH(),
											r,
											sX*((float)u->getHealth()/u->getMaxHealth()),
											sY,
											1);
		list->addRenderItem(	containerID,
											((int)x-vX),
											((int)y-vY),
											0,
											(int)a,
											getW(),
											getH(),
											r,
											sX,
											sY,
											1);
	}
	virtual FXParticle* clone(){
		UnitHPBar* newParticle = new UnitHPBar();
		newParticle->runTime=runTime;
		newParticle->containerID=containerID;
		newParticle->key=key;
		return newParticle;
	}
	virtual void recycle(){
		x=y=vx=vy=va=vsX=vsY=r=vR=0;
		sX=sY=1;
		a=255;
	}
	virtual bool update(){
		if(u==NULL)return true;
		x=(float)u->getX()-offx;
		y=(float)u->getY()-offy;
		if(u->isDead())return true;
		return false;
	}
};