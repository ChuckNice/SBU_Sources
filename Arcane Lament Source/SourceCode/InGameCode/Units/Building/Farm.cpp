#include <SourceCode\InGameCode\Units\Building\Farm.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SourceCode\InGameCode\InGameState.h>
#include <SourceCode\ProjectBase\ResourceBank.h>

float Farm::goldCost=0,
	Farm::influenceRange=0,
	Farm::constructionPercentBase=0,
	Farm::constructionPercentIncrement=0;


float Farm::hpScaleX,Farm::hpScaleY;
float Farm::hpOffX,Farm::hpOffY;

float Farm::maxHealthBase, Farm::armorBase;


wstring Farm::typeID= L"Farm";

Bot* Farm::clone()
{
		Farm *b = new Farm();
		b->setSpriteType(getSpriteType());
		b->setCurrentState(wstring(L"IDLE_STATE"));
		b->w=w= (int)(std::ceil((float)(getSpriteType()->getPhysicalW()/TILE_WIDTH)));
		b->h=h= (int)(std::ceil((float)(getSpriteType()->getPhysicalH()/TILE_HEIGHT)));
		return b;
}
void Farm::mouseClick(Game* g, int x, int y){
		if(workers++>=5 || p->pop<20){
			p->pop+=(workers-1)*20;
			workers=0;
		}else{
			p->pop-=20;
		}
}


void Farm::loadSelf(lua_State* s){
	luabind::object table = luabind::globals(s)["UnitTypeConstants"]["Farm"];
}

void Farm::render(RenderList* list,Viewport* vp){
	Unit::render(list,vp);
	if(constructionPercent<100){
		float x=(body->GetPosition().x*BOX2D_Scale_Factor_INV),y=(body->GetPosition().y*BOX2D_Scale_Factor_INV);
		float l= x+getSpriteType()->getOffsetX(),
			  t= y+getSpriteType()->getOffsetY();
			x-=vp->getViewportX();
			y-=vp->getViewportY();
			l-=vp->getViewportX();
			t-=vp->getViewportY();
			int icon =ResourceBankgetResource((string)"PopIconWorld");
			for(int i=0;i<workers;i++){
				list->addRenderItem(icon, 
					(int)l+8+4*(workers-i),
					(int)t+8,
					0, 
					getAlpha(),
					64,
					64,
					getRotTransform(),
					.5f,.5f
					);
			}
			float scale = constructionPercent/100;
			list->addRenderItem(barFill,
				(int)l,
				(int)t+10,
				0,
				getAlpha(),
				64,
				64,
				getRotTransform(),scale*.5f,1,1
				);
			list->addRenderItem(barOutline,
				(int)l,
				(int)t+10,
				0,
				getAlpha(),
				64,
				64,
				getRotTransform(),
				.5f,.5f,1
				);
		}
	else if(workers>0){
		p->pop+=workers*20;
		workers=0;
	}
}

bool Farm::think(Game *game){
	if(alertCooldown>0)alertCooldown--;
	if(!isBuilt && constructionPercent>=100){
		p->pgrPerPOP+=.005;
		isBuilt=true;
	}
	if(isDead()){
		if(constructionPercent>=100){
			p->pgrPerPOP-=.005;
		}
		return true;

	}
	if(constructionPercent<100){
		constructionPercent+=constructionPercentBase+this->constructionPercentIncrement *workers;
	}
		return false;
	}



	bool Farm::canBuild(Game* g, InGameState *state, Player* p){
		if(p->gold<this->goldCost)
			return false;
		return true;
	}