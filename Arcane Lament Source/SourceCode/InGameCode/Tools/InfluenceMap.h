#pragma once
class Player;
class Tower;
class Building;
#include <stdafx.h>
class InfluenceMap{
	int currentMask;
	map<Player*, int> bitMasks;
	int **grid;
public:
	int w,h,tileW,tileH;
	InfluenceMap(){}
	InfluenceMap(int numTilesHorizontal, int numTilesVertical, int tileW, int tileH){
		this->w=numTilesHorizontal;
		this->h=numTilesVertical;
		this->tileW=tileW;
		this->tileH=tileH;
		currentMask=1;
		grid= new int*[w];
		for(int i=0;i<w;i++){
			grid[i]=new int[h];
		}
	}
	int isRegistered(Player* p){
		map<Player* ,int>::iterator find = bitMasks.find(p);
		if(find==bitMasks.end())return 0;
		return find->second;
	}
	void registerPlayer(Player* p);
	bool doesPlayerHaveInfluenceHere(Player* p,float screenX, float screenY);
	void recalculateInfluences(vector<Tower*> towerlist ,vector<Building*> list);
	template <class T>
	void recalculateInfluences(vector<T*> list);
	void clearInfluence(){
		for(int i=0;i<w;i++)
			for(int j=0;j<h;j++)
				grid[i][j]=0;
	}
};