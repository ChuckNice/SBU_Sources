#pragma once
#define MAXGRIDDATA 1000
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SourceCode\InGameCode\StatusFX.h>
#include <stdafx.h>
class World;
class Game;
class waypoint;
class GridExaminer{
	World* w;
	float tileWidth,tileHeight;
public:
	short** gridData;
	int width,height,xBound,yBound;
	static StatusFactory<waypoint> waypointFactory;
	GridExaminer(World* wrld,int tileWidth, int tileHeight){
		w=wrld;
		this->tileWidth=tileWidth*BOX2D_Scale_Factor;
		this->tileHeight=tileHeight*BOX2D_Scale_Factor;
		gridData=new short*[MAXGRIDDATA];
		for(int i=0;i<MAXGRIDDATA;i++)
			gridData[i]= new short[MAXGRIDDATA];
	}
	~GridExaminer(){
		for(int i=0;i<MAXGRIDDATA;i++)
			delete[] gridData[i];
		delete[] gridData;
	}

	void aStarAlgorithm(Game* g,int xi,int yi,int xf,int yf,vector<pair<float,float>> *in);
	void evaluateGrid(Game* g);
};