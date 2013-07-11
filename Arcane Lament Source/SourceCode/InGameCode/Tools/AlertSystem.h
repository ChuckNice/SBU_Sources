#pragma once
#include <stdafx.h>

class Player;
struct Alert{
	Player* publisher;
	float x, y;
	long time;
};

class AlertSystem{
	list<Alert> alerts;
	long time;
	AlertSystem():time(0){}
	void publishAlert(float x, float y, Player* p){
		Alert a;
		a.x=x;
		a.y=y;
		a.publisher=p;
		a.time=time+120;
		alerts.push_back(a);
	}
	void update(){
		time++;
		if(!alerts.empty()){
			while(alerts.front().time<=time)
				alerts.pop_front();
		}

	}
	void clearAlerts(){
		alerts.clear();
	}
};