#pragma once
#include <SourceCode\InGameCode\Unit.h>
class SensorListener{
public:
	virtual void isWithinSensor(Unit* inRange){}
};