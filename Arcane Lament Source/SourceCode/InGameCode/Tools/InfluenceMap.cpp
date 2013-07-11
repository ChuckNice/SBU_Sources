#include <SourceCode\InGameCode\Tools\InfluenceMap.h>
#include <SourceCode\InGameCode\Player.h>
#include <SourceCode\InGameCode\Units\Tower.h>
#include <SourceCode\InGameCode\Building.h>

void InfluenceMap::registerPlayer(Player* p){
	bitMasks[p]=currentMask;
	currentMask=currentMask<<1;
}
bool InfluenceMap::doesPlayerHaveInfluenceHere(Player* p,float screenX, float screenY){
	screenX/=tileW;
	screenY/=tileH;
	map<Player* ,int>::iterator find = bitMasks.find(p);
	if(find==bitMasks.end())return false;
	int mask = find->second;
	return (mask&grid[(int)screenX][(int)screenY])>0;
}
void InfluenceMap::recalculateInfluences(vector<Tower*> towerlist ,vector<Building*> buildinglist){
	recalculateInfluences<Tower>(towerlist);
	recalculateInfluences<Building>(buildinglist);
}


template <class T>
void InfluenceMap::recalculateInfluences(vector<T*> list){
	vector<T*>::iterator begin= list.begin(), end= list.end();
	for(;begin!=end;begin++)
	{
		int mask =isRegistered((*begin)->p);
		if(mask==0)continue;
		int range=(*begin)->getInfluenceRange();
		int x = (int)(*begin)->getX()/tileW;
		int y = (int)(*begin)->getY()/tileH;
		for(int i=max(x-range,0);i<min(x+range,w);i++)
			for(int j=max(y-range,0);j<min(y+range,h);j++)
				grid[i][j]|=mask;
	}
}