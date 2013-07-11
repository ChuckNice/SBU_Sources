#include <SourceCode\InGameCode\Spells\Fireball.h>
#include <SourceCode\InGameCode\Unit.h>
#include <SourceCode\InGameCode\FX\GenericFXParticles.h>
#include <SourceCode\InGameCode\Player.h>
class FireballQuery:public b2QueryCallback{
	
	public:
	Player* me;
	b2Body* myBody;
	bool ReportFixture(b2Fixture* fixture)
	{
		b2Body* body = fixture->GetBody();
		if(fixture->IsSensor())return true;
		ActorHook* data = (ActorHook*)body->GetUserData();
		if(data->type==UNIT_TYPE){
			Unit* u = (Unit*)data->data;
			if(!me->isAlly(u)){
				u->damageUnit(10);
				u->disturb(10);
				b2Vec2 d(body->GetPosition().x-myBody->GetPosition().x,body->GetPosition().y-myBody->GetPosition().y);
				d.Normalize();
				d*=50;
				body->ApplyForceToCenter(d);
			}
		}
		return true;
	}
};

void Fireball::onProjectileContact(b2Body* yours, b2Body* contact){
		/*ActorHook* data = (ActorHook*)contact->GetUserData();
		if(data->type==UNIT_TYPE){
			Unit* u = (Unit*)data->data;
			if(!owner->isAlly(u)){
				explode=true;
			}
		}else */
			explode=true;
	}


bool Fireball::update(Game* game){
		if(explode){
			b2World* world=game->getGSM()->getPhysics()->getWorld();
			FireballQuery q;
			q.me=owner;
			q.myBody=this->self;
			b2AABB box;
			box.lowerBound= b2Vec2(self->GetPosition().x-100*BOX2D_Scale_Factor,self->GetPosition().y-100*BOX2D_Scale_Factor);
			box.upperBound= b2Vec2(self->GetPosition().x+100*BOX2D_Scale_Factor,self->GetPosition().y+100*BOX2D_Scale_Factor);
			world->QueryAABB(&q,box);
			FXParticle* p =game->getGSM()->getFXSys()->getParticle(EXPLOSION);
			p->x=self->GetPosition().x*BOX2D_Scale_Factor_INV;
			p->y=self->GetPosition().y*BOX2D_Scale_Factor_INV;
			p->vsX=p->vsY=.05f;
			game->getGSM()->getWorld()->addParticle(p);
			world->DestroyBody(self);
			return true;
		}
		return false;
	}


	void Fireball::renderPreview(RenderList *l, int xi,int yi, int xf, int yf, int vx, int vy){
		l->addRenderItem(Spell::SpellCircleID,xi-vx-16,yi-vy-16,0,255,0,0);
		float dx = xf-xi, dy=yf-yi;
		float r = atan2((float)dy,dx);
		float dist = sqrt(dx*dx+dy*dy);
		l->addRenderItem(Spell::SpellLineID,xi-vx,yi-vy,0,255,0,0,r,dist/15,1,1);
		l->addRenderItem(Spell::SpellArrowID,xf-vx-8,yf-vy-8,0,255,0,0,r+3.14/4);

	}