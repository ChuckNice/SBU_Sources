#include <SourceCode\InGameCode\Units\Building\Altar.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SourceCode\InGameCode\InGameState.h>
#include <SourceCode\ProjectBase\ResourceBank.h>

float Altar::goldCost=0,
	Altar::influenceRange=0,
	Altar::constructionPercentBase=0,
	Altar::constructionPercentIncrement=0;


float Altar::hpScaleX,Altar::hpScaleY;
float Altar::hpOffX,Altar::hpOffY;


float Altar::maxHealthBase, Altar::armorBase;

float Altar::manaPerWorker,
	Altar::manaBase;


wstring Altar::typeID= L"Altar";

Bot* Altar::clone()
{
		Altar *b = new Altar();
		b->setSpriteType(getSpriteType());
		b->setCurrentState(wstring(L"IDLE_STATE"));
		b->w=w= (int)(std::ceil((float)(getSpriteType()->getPhysicalW()/TILE_WIDTH)));
		b->h=h= (int)(std::ceil((float)(getSpriteType()->getPhysicalH()/TILE_HEIGHT)));
		return b;
}
void Altar::mouseClick(Game* g, int x, int y){
		if(workers++>=5 || p->pop<20){
			p->pop+=(workers-1)*20;
			if(isBuilt){
				manaBalance-=(workers-1)*manaPerWorker;
				p->manaIncome-=(workers-1)*manaPerWorker;
			}
			workers=0;
		}else{
			p->pop-=20;
			if(isBuilt){
				manaBalance+=manaPerWorker;
				p->manaIncome+=manaPerWorker;
			}
		}
}


void Altar::loadSelf(lua_State* s){
	luabind::object table = luabind::globals(s)["UnitTypeConstants"]["Altar"];
	manaBase=luabind::object_cast<float>(table["manaBase"]);
	manaPerWorker=luabind::object_cast<float>(table["manaPerWorker"]);
}

void Altar::render(RenderList* list,Viewport* vp){
	Unit::render(list,vp);
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
		if(!isBuilt){
			float scale = constructionPercent/100;
			list->addRenderItem(barFill,
				(int)l,
				(int)t+10,
				0,
				getAlpha(),
				64,
				64,
				getRotTransform(),scale,1,1
				);
			list->addRenderItem(barOutline,
				(int)l,
				(int)t+10,
				0,
				getAlpha(),
				64,
				64,
				getRotTransform()
				);
		}
}

bool Altar::think(Game *game){
	if(alertCooldown>0)alertCooldown--;
	if(isDead()){
			p->manaIncome-=manaBalance;
			return true;
		}
	if(constructionPercent<100){
		constructionPercent+=constructionPercentBase+this->constructionPercentIncrement *workers;
	}else{
		if(!isBuilt){
			isBuilt=true;
			p->pop+=workers*20;
			manaBalance+=manaBase;
			p->manaIncome+=manaBase;
			workers=0;
		}
		
	}
	
		return false;
	}



	bool Altar::canBuild(Game* g, InGameState *state, Player* p){
		if(p->gold<this->goldCost)
			return false;
		return true;
	}