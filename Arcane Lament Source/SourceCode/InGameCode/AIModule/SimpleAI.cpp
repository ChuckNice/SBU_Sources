#include <SourceCode\InGameCode\AIModule\SimpleAI.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <SourceCode\InGameCode\Player.h>
#include <SSSF_SourceCode\gsm\physics\ActorHook.h>
#include <SourceCode\InGameCode\Unit.h>
#include <SourceCode\InGameCode\InGameState.h>
#include <SourceCode\InGameCode\Units\Building\Barracks.h>
#include <SourceCode\InGameCode\Units\WizardUnit.h>
#include <SourceCode\InGameCode\Units\Knight.h>
#include <SourceCode\InGameCode\Units\Archer.h>
#include <SourceCode\InGameCode\Units\Ogre.h>
#include <SourceCode\InGameCode\Units\Building\ScoutTower.h>
#include <SourceCode\InGameCode\Units\Building\Farm.h>
#include <SourceCode\InGameCode\Units\Building\Altar.h>
#include <SourceCode\InGameCode\SpellDB.h>
void chooseUnits(Player* me){
	int myWar=me->numWarriorsCreated-me->numWarriorsLost,
		myA=me->numArchersCreated-me->numArchersLost,
		myK=me->numKnightsCreated-me->numKnightsLost,
		myWiz=me->numWizardsCreated-me->numWizardsLost,
		myO=me->numOgresCreated-me->numOgresLost;
	int enemyWar=me->enemy->numWarriorsCreated-me->enemy->numWarriorsLost,
		enemyA=me->enemy->numArchersCreated-me->enemy->numArchersLost,
		enemyK=me->enemy->numKnightsCreated-me->enemy->numKnightsLost,
		enemyWiz=me->enemy->numWizardsCreated-me->enemy->numWizardsLost,
		enemyO=me->enemy->numOgresCreated-me->enemy->numOgresLost;
	int wantWar,wantA,wantK,wantWiz,wantO;
	wantWar=max(enemyWiz-myWar,0)+1;
	wantA=max(enemyO-myA*10,0)+2;
	wantK=(int)max(.4f*(enemyWar+enemyA)-myK,0)+3;
	wantWiz=(int)max(enemyK-myWiz,0)+5;
	wantO=(int)10;
	int wantTotal= wantWar+wantA+wantK+wantWiz+wantO;
	while(me->gold>5&&me->buildStack.size()<80){ 
		me->queueUnit(0,0,(int)ceil(5*(float)wantWar/wantTotal));
		me->queueUnit(1,0,(int)ceil(5*(float)wantA/wantTotal));
		me->queueUnit(2,0,(int)ceil(5*(float)wantK/wantTotal));
		me->queueUnit(3,0,(int)ceil(5*(float)wantWiz/wantTotal));
		me->queueUnit(4,0,(int)ceil(5*(float)wantO/wantTotal)); 
	}
}
void SimpleAI::think(InGameState *s, Game* g, Player* me){
	if(lastCreateClusters++>50){
		constructClusters(s,g,me);
		lastCreateClusters=0;
		vector<Cluster>::iterator begin=existingClusters.begin(),end=existingClusters.end();
		int largest=0;;
		Cluster *c=NULL;
		for(;begin!=end;begin++){
			begin->x+=begin->vX;
			begin->y+=begin->vY;
			if(begin->strength>largest){
				largest=begin->strength;
				c=&*begin;
			}
		}
		vector<string> spells;
		spells.push_back("Fireball");
		spells.push_back("Meteor");
		spells.push_back("KineticBooster");
		spells.push_back("MeteorShower");
		spells.push_back("GravityWell");
		if(c!=NULL && me->armyStrength<me->enemy->armyStrength*.5f && me->mana>10){
		
			int num=rand()%spells.size();
			string mySpell=spells[num];
			Spell* newSpell = s->getSpellDB()->getSpell(mySpell)->clone();
			newSpell->owner=me;
			me->mana-=10;
			if(mySpell=="Fireball"||mySpell=="KineticBooster"){
				newSpell->start(g,(c->x+5*c->vX+(rand()%5-2))*BOX2D_Scale_Factor_INV,(c->y+5*c->vY+(rand()%5-2))*BOX2D_Scale_Factor_INV,
					(c->x)*BOX2D_Scale_Factor_INV,(c->y)*BOX2D_Scale_Factor_INV);
				s->spells.push_back(newSpell);
			}
			if(mySpell=="GravityWell"){
				newSpell->start(g,(c->x-5*c->vX)*BOX2D_Scale_Factor_INV,(c->y-5*c->vY)*BOX2D_Scale_Factor_INV);
				s->spells.push_back(newSpell);
			}
			if(mySpell=="Meteor"||mySpell=="MeteorShower"){
				newSpell->start(g,(c->x+5*c->vX)*BOX2D_Scale_Factor_INV,(c->y+5*c->vY)*BOX2D_Scale_Factor_INV);
				s->spells.push_back(newSpell);
			}
		}
	}else{
		checkClusters(s,g,me);
		//countUnitsAndDispatchOrders(s,g,me);
		if(me->buildStack.empty()){
			chooseUnits(me);
		}
	}
}
class b2CountUnits:public b2QueryCallback{
public:
	int n;
	float str, avgX, avgY, density;
	float maxX,maxY,minX,minY;
	vector<Unit*> units;
	Player* p;
	bool ReportFixture(b2Fixture* fixture)
		{
			if(fixture->IsSensor())return true;
			b2Body* body = fixture->GetBody();
			ActorHook* hook = (ActorHook*)body->GetUserData();
			if(hook->type==UNIT_TYPE){
				Unit* givenUnit = (Unit*) hook->data;
				if(!p->isAlly(givenUnit)){
					str+=givenUnit->strengthEstimate();
					float x = givenUnit->getWorldX(), y=givenUnit->getWorldY();
					avgX+=x;
					avgY+=y;
					if(minX==-1)minX=maxX=x;
					if(minY==-1)minY=maxY=y;
					if(x<minX)minX=x;
					else if (x>maxX)x=maxX;
					if(y<minY)minY=y;
					else if (y>maxY)y=maxY;
					n++;
					units.push_back(givenUnit);
				}
			}
			return true;
		}
};

void splitSearchArea(Player* me, float l, float t, float r, float b, Game* g, vector<Cluster>* clusters,float density){
	if(r-l<3 || b-t<3)return;
	b2CountUnits search;
	search.n=0;
	search.str=search.avgX=search.avgY=search.density=0;
	search.minX=search.minY=-1;
	search.p=me;
	b2AABB rct;
	rct.lowerBound=b2Vec2(l,t);
	rct.upperBound=b2Vec2(r,b);
	g->getGSM()->getPhysics()->getWorld()->QueryAABB(&search,rct);
	search.avgX/=search.n;
	search.avgY/=search.n;
	float avgRadius=0;
	for(int i=0;i<search.units.size();i++){
		Unit* u = search.units.at(i);
		avgRadius+= sqrt(pow(u->getWorldX()-search.avgX,2)+pow(u->getWorldY()-search.avgY,2));
	}
	avgRadius/=search.n;
	float area = 3.14*avgRadius*avgRadius;
	if(area==0){area=5;
		avgRadius=2;
	}
	if(area!=0)search.density=(float)search.str/area;
	else search.density=0;
	
	if(search.density>density){
		for(int i=0;i<clusters->size();i++){
			Cluster other = clusters->at(i);
			float dist = sqrt(pow(other.x-search.avgX,2)+pow(other.y-search.avgY,2));
			if(dist<other.radius+avgRadius)return;
		}
		Cluster c;
		c.x=search.avgX;
		c.y=search.avgY;
		c.strength=search.str;
		c.friendly=false;
		c.radius=avgRadius;
		float vx=0,vy=0;
		float scale=g->getGSM()->getPhysics()->getTimeStep();
		for(int i=0;i<search.units.size();i++){
			Unit* u = search.units.at(i);
			b2Vec2 v=u->getBody()->GetLinearVelocity();
			vx+=v.x;
			vy+=v.y;
		}
		vx/=search.n;
		vy/=search.n;
		c.vX=vx*scale;
		c.vY=vy*scale;
		clusters->push_back(c);
		
	}else{
		if(search.str<3)return;
		splitSearchArea(me,l,t,(l+r)/2,(t+b)/2,g,clusters,density);
		splitSearchArea(me,(l+r)/2,(t+b)/2,r,b,g,clusters,density);
		splitSearchArea(me,l,(t+b)/2,(l+r)/2,b,g,clusters,density);
		splitSearchArea(me,(l+r)/2,t,r,(t+b)/2,g,clusters,density);
	}
}

void SimpleAI::constructClusters(InGameState *s, Game* g, Player* me){
	existingClusters.clear();
	splitSearchArea(me,g->getGSM()->getPhysics()->l,g->getGSM()->getPhysics()->t,g->getGSM()->getPhysics()->r,g->getGSM()->getPhysics()->b,
		g,&existingClusters,.7f);
}
void SimpleAI::checkClusters(InGameState *s, Game* g, Player* me){
	vector<Cluster>::iterator begin=existingClusters.begin(),end=existingClusters.end();
	int largest=0;;
	Cluster *c=NULL;
	for(;begin!=end;begin++){
		begin->x+=begin->vX;
		begin->y+=begin->vY;
		if(begin->strength>largest){
			largest=begin->strength;
			c=&*begin;
		}
	}
	
}

 

void SimpleAI::countUnitsAndDispatchOrders(InGameState *s, Game* g, Player* me){
	myPriorities.numWarriors=myPriorities.numArchers=myPriorities.numKnights=myPriorities.numWizards=myPriorities.numOgres=0;
	myArmy.clear();
	list<Bot*>::iterator begin = g->getGSM()->getSpriteManager()->getBotList()->begin(),
		end= g->getGSM()->getSpriteManager()->getBotList()->end();
	for(;begin!=end;begin++){
		if(((ActorHook*)((*begin)->getBody()->GetUserData()))->type==UNIT_TYPE){
			Unit* u = (Unit*)*begin;
			if(me==u->p){
				if(typeid(*u)==typeid(WizardUnit))
					myPriorities.numWizards++;
				else if(typeid(*u)==typeid(Warrior))
					myPriorities.numWarriors++;
				else if(typeid(*u)==typeid(Ogre))
					myPriorities.numOgres++;
				else if(typeid(*u)==typeid(Archer))
					myPriorities.numArchers++;
				else if(typeid(*u)==typeid(Knight))
					myPriorities.numKnights++;
				myArmy.push_back(u);
			}
		}
	}
	

}