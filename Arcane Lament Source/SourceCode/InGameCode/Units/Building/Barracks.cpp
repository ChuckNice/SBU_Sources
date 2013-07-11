#include <SourceCode\InGameCode\Units\Building\Barracks.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SourceCode\InGameCode\InGameState.h>
#include <SourceCode\ProjectBase\ResourceBank.h>
#include <SourceCode\InGameCode\GUI\BarracksGUI.h>

float Barracks::goldCost=0,
	Barracks::influenceRange=0,
	Barracks::constructionPercentBase=0,
	Barracks::constructionPercentIncrement=0;

float Barracks::maxHealthBase, Barracks::armorBase;

float Barracks::hpScaleX,Barracks::hpScaleY;
float Barracks::hpOffX,Barracks::hpOffY;


wstring Barracks::typeID= L"Barracks";

Bot* Barracks::clone()
{
		Barracks *b = new Barracks();
		b->setSpriteType(getSpriteType());
		b->setCurrentState(wstring(L"IDLE_STATE"));
		b->w=w= (int)(std::ceil((float)(getSpriteType()->getPhysicalW()/TILE_WIDTH)));
		b->h=h= (int)(std::ceil((float)(getSpriteType()->getPhysicalH()/TILE_HEIGHT)));
		return b;
}
void Barracks::mouseClick(Game* g, int x, int y){
	
	if(constructionPercent>100){
	}else{
		if(workers++>=5 || p->pop<20){
			p->pop+=(workers-1)*20;
			workers=0;
		}else{
			p->pop-=20;
		}
	}
}


void Barracks::render(RenderList* list,Viewport* vp){
	Unit::render(list,vp);
	if(constructionPercent>=100){
		if(workers>0){
			p->pop+=workers*20;
			workers=0;
		}
		if(maxPercentage>0){
			float x=(body->GetPosition().x*BOX2D_Scale_Factor_INV),y=(body->GetPosition().y*BOX2D_Scale_Factor_INV);
			float l= x+getSpriteType()->getOffsetX(),
				  t= y+getSpriteType()->getOffsetY();
				x-=vp->getViewportX();
				y-=vp->getViewportY();
				l-=vp->getViewportX();
				t-=vp->getViewportY();
				float scale = (float)currentPercent/maxPercentage;
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
		return;
	}
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

bool Barracks::think(Game *game){
	if(alertCooldown>0)alertCooldown--;
	if(constructionPercent<100){
		constructionPercent+=constructionPercentBase+this->constructionPercentIncrement *workers;
	}
		if(isDead()){ 
			p->numBarracks--;
			return true;
		}
		
		return false;
	}


	void Barracks::loadSelf(lua_State* s){

	}

	bool Barracks::canBuild(Game* g, InGameState *state, Player* p){
		if(p->gold<this->goldCost)
			return false;
		return true;
	}