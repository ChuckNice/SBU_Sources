#pragma once
#include <list>
class Game;
class RenderList;
class Unit;
class StatusFX{
public:
	virtual void birth(Game* g){}
	virtual void applyToUnit(Unit* u){}
	virtual void render(RenderList* list, int x,int y){}
	virtual inline bool isDestroyed(){return true;}
	virtual void release(){}
	virtual bool stack(StatusFX* fx){return true;}
};

template <class T>
class StatusFactory{
	std::list<void*> recycled;
public:
	T* get(){
		if(recycled.empty()){
			for(int i=0;i<10;i++)
				recycled.push_back(new T());
		}
		T* item = (T*)recycled.back();
		recycled.pop_back();
		return item;
	}
	void recycle(T* item){
		recycled.push_front(item);
	}
};