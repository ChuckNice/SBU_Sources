#pragma once
#include "stdafx.h"
class Unit;
class  AttackCallback:public b2QueryCallback{
	public:
		Unit *myUnit;
		bool ReportFixture(b2Fixture* fixture);
	};