#pragma once
#include <Box2D\Box2D.h>
#include "stdafx.h"
class Unit;
struct CallbackFunctor{
	virtual bool operator()(Unit* self, Unit* other)=0;
};
template<class functor>
class  AABBCallback:public b2QueryCallback{
public:
	functor *f;
	Unit* self;
	bool ReportFixture(b2Fixture* fixture){
		if(fixture->IsSensor())return true;
			b2Body* body = fixture->GetBody();
			ActorHook* data = (ActorHook*)body->GetUserData();
			if(data->type==UNIT_TYPE||data->type==BUILDING_TYPE){
				Unit* inRange = (Unit*)data->data;
				return (*f)(self,inRange);
			}
			return true;
	}
};