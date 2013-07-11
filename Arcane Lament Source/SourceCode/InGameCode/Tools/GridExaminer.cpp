#include <SourceCode\InGameCode\Tools\GridExaminer.h>
#include <SSSF_SourceCode\gsm\world\World.h>
#include <SSSF_SourceCode\gsm\world\WorldLayer.h>
#include <SSSF_SourceCode\gsm\world\SparseLayer.h>
#include <SourceCode\ProjectBase\GroupGameData.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <Box2D\Box2D.h>

StatusFactory<waypoint> GridExaminer::waypointFactory;

class ContainsObstacles:public b2QueryCallback{
public:
	bool* flag;
	bool ReportFixture(b2Fixture* fixture)
		{
			if(fixture->IsSensor())return true;
			b2Body* body = fixture->GetBody();
			if(body->GetType()==b2_staticBody){
				*flag=true;
				return false;
			}
			return true;
		}
};


class CountObstacles:public b2QueryCallback{
public:
	float* flag;
	int increment;
	bool ReportFixture(b2Fixture* fixture)
		{
			if(fixture->IsSensor())return true;
			b2Body* body = fixture->GetBody();
			if(body->GetType()==b2_staticBody){
				*flag+=increment;
			}
			return true;
		}
};

class RayCastObstacles:public b2RayCastCallback{
public:
	bool* flag;
	 float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
									  const b2Vec2& normal, float32 fraction)
	{	
		if(fraction>.85f)return -1.0f;
		if(fixture->IsSensor())return -1.0f;
			b2Body* body = fixture->GetBody();
			if(body->GetType()==b2_staticBody){
				*flag=true;
				return 0.0f;
			}
		return -1.0f;
	}
};


struct waypoint{
	int xi,yi;
	int pathCost,cost, totalCost;
	waypoint(){}
	waypoint(int x,int y):xi(x),yi(y),cost(0){}
	waypoint(int x,int y,waypoint* p):xi(x),yi(y),parent(p),cost(0){}
	waypoint(int x,int y,waypoint* p,int c):xi(x),yi(y),parent(p),cost(c){}
	waypoint *parent;
};
inline bool waypointLowestCost(waypoint *wp1,waypoint *wp2){
	return wp1->totalCost>wp2->totalCost;
}
void addWayPoint(int x,int y, int pathCost,int eC,waypoint* parent,vector<waypoint*>* list){
	/*for(;start!=end;start++)
	{
		waypoint a = **start;
		if(a.xi==x && a.yi==y){
			if(a.pathCost>pathCost){
				a.parent=parent;
				a.pathCost=pathCost;
				a.totalCost=a.cost+pathCost;
			}
			return;
		}
	}*/
	waypoint * p=GridExaminer::waypointFactory.get();
	p->xi=x;
	p->yi=y;
	p->pathCost=pathCost;
	p->cost=eC;
	p->totalCost=p->pathCost+p->cost;
	p->parent=parent; 
	list->push_back(p);push_heap(list->begin(),list->end(),&waypointLowestCost);
}
bool listContains(vector<waypoint*>* list, int xi, int yi){
	vector<waypoint*>::iterator start=list->begin(),end=list->end();
	for(;start!=end;start++)
	{
		if((*start)->xi==xi && (*start)->yi==yi){
			return true;
		}
	}
	return false;
}
bool testArea(Game* g,int x, int y, int w, int h){
	b2AABB queryArea; 
	bool isFilled=false;
	ContainsObstacles callback;
	callback.flag=&isFilled;
	queryArea.lowerBound.x=(x)*w+1; 
	queryArea.lowerBound.y=(y)*h+1;
	queryArea.upperBound.x=(x+1)*w-1;
	queryArea.upperBound.y=(y+1)*h-1;
	g->getGSM()->getPhysics()->getWorld()->QueryAABB(&callback,queryArea);
	return isFilled;
	
}
float testCostArea(Game* g,int x, int y, int w, int h){
	b2AABB queryArea; 
	float cost=0;
	CountObstacles callback;
	callback.flag=&cost;
	queryArea.lowerBound.x=(x)*w+1; 
	queryArea.lowerBound.y=(y)*h+1;
	queryArea.upperBound.x=(x+1)*w-1;
	queryArea.upperBound.y=(y+1)*h-1;
	g->getGSM()->getPhysics()->getWorld()->QueryAABB(&callback,queryArea);
	return cost;
	
}

void testArea(GridExaminer* grid,Game* g,int x, int y,int fx, int fy,int w, int h,int eC,vector<waypoint*>* list, vector<waypoint*>* closedlist,waypoint *parent){
	if(x<0||y<0||x>grid->xBound||y>grid->yBound)return;
	/*b2AABB queryArea; 
	bool isFilled=false;
	ContainsObstacles callback;
	callback.flag=&isFilled;*/
	if(!listContains(closedlist,x,y)){
		/*queryArea.lowerBound.x=(x)*w+1; 
		queryArea.lowerBound.y=(y)*h+1;
		queryArea.upperBound.x=(x+1)*w-1;
		queryArea.upperBound.y=(y+1)*h-1;
		g->getGSM()->getPhysics()->getWorld()->QueryAABB(&callback,queryArea);*/
		//if(!isFilled){
		addWayPoint(x,y,parent->pathCost+eC,/*testCostArea(g,x,y,w,h)+*/grid->gridData[x][y]+abs(fx-x)*10+abs(fy-y)*10,parent,list);
		//}
	}
}
void populateNearby(GridExaminer * grid,Game* g, int x, int y,int fx,int fy,int w,int h, vector<waypoint*>* list, vector<waypoint*>* closedlist,waypoint *parent){
	testArea(grid,g,x+1,y+1,fx,fy,w,h,14,list,closedlist,parent);
	testArea(grid,g,x+1,y-1,fx,fy,w,h,14,list,closedlist,parent);
	testArea(grid,g,x-1,y-1,fx,fy,w,h,14,list,closedlist,parent);
	testArea(grid,g,x-1,y+1,fx,fy,w,h,14,list,closedlist,parent);
	testArea(grid,g,x,y+1,fx,fy,w,h,10,list,closedlist,parent);
	testArea(grid,g,x,y-1,fx,fy,w,h,10,list,closedlist,parent);
	testArea(grid,g,x+1,y,fx,fy,w,h,10,list,closedlist,parent);
	testArea(grid,g,x-1,y,fx,fy,w,h,10,list,closedlist,parent);
}
void GridExaminer::aStarAlgorithm(Game* g,int xi,int yi,int xf,int yf, vector<pair<float,float>> *waypoints){
	int xTileI=xi/tileWidth,yTileI=yi/tileHeight,xTileF=xf/tileWidth,yTileF=yf/tileHeight;
	
	bool flag =false;
	RayCastObstacles raycast;
	raycast.flag=&flag;
	b2Vec2 a(xi,yi), b(xf,yf);
	g->getGSM()->getPhysics()->getWorld()->RayCast(&raycast,a,b);
	if(!flag){
		waypoints->clear();
		waypoints->push_back(make_pair(xf,yf));
		return;
	}

	vector<waypoint*> openList,closedList;
	waypoint *p =waypointFactory.get();
	p->xi=xTileI;p->yi=yTileI;
	p->totalCost=0;
	p->pathCost=0;
	p->cost=0;
	p->parent=NULL;
	openList.push_back(p);
	waypoint *tst;
	int maxTest = ((xTileF-xTileI)*(xTileF-xTileI)+(yTileF-yTileI)*(yTileF-yTileI))/2;
	int leniency = maxTest<20?0:5;
	//Loop that checks performs the algorithm
	while(!openList.empty()){
		//std::sort(openList.begin(),openList.end(),&waypointLowestCost);
		pop_heap(openList.begin(),openList.end(),&waypointLowestCost);	
		tst= openList.back();
		openList.pop_back();
		if(abs(tst->xi-xTileF)<=leniency&&abs(tst->yi-yTileF)<=leniency){
			break;
		}
		else if(closedList.size()>maxTest){
			break;
		}
		else{
			populateNearby(this,g,tst->xi,tst->yi,xTileF,yTileF,tileWidth,tileHeight,&openList,&closedList,tst);
			closedList.push_back(tst);
		}
	}
	//Shrink list of waypoints to exlude points in a straight line. Also converts waypoints to natural box2d coords
	waypoints->clear();
	waypoint* cursor=tst;
	waypoints->push_back(make_pair(cursor->xi*tileWidth,cursor->yi*tileHeight));
	int dx=-5,dy=-5;
	if(tst->parent!=NULL){
		dx=tst->xi-tst->parent->xi;
		dy=tst->yi-tst->parent->yi;
	}
	while(cursor->parent!=NULL){
		int ndx=cursor->xi-cursor->parent->xi;
		int ndy=cursor->yi-cursor->parent->yi;
		if(ndx!=dx||ndy!=dy){
			waypoints->push_back(make_pair(cursor->xi*tileWidth,cursor->yi*tileHeight));
			dx=ndx;
			dy=ndy;
		}
		cursor=cursor->parent;
	}
	//Clean up
	vector<waypoint*>::iterator start=openList.begin(),end=openList.end();
	for(;start!=end;start++)
		waypointFactory.recycle(*start);
	openList.clear();
	start=closedList.begin(),end=closedList.end();
	for(;start!=end;start++)
		waypointFactory.recycle(*start);
	closedList.clear();
}
void GridExaminer::evaluateGrid(Game* g){
	width=w->getWorldWidth();
	height=w->getWorldHeight();
	xBound = width/tileWidth*BOX2D_Scale_Factor;
	yBound = height/tileHeight*BOX2D_Scale_Factor;
	b2AABB queryArea;
	for(int i=0;i<xBound;i++){
		for(int j=0;j<yBound;j++){ 
			queryArea.lowerBound.x=i*tileWidth+1;
			queryArea.lowerBound.y=j*tileHeight+1;
			queryArea.upperBound.x=(i+1)*tileWidth-1;
			queryArea.upperBound.y=(j+1)*tileHeight-1;
			float obstacles=0;
			CountObstacles callback;
			callback.increment=100;
			callback.flag=&obstacles;
			g->getGSM()->getPhysics()->getWorld()->QueryAABB(&callback,queryArea);
			queryArea.lowerBound.x=(i-1)*tileWidth+1;
			queryArea.lowerBound.y=(j-1)*tileHeight+1;
			queryArea.upperBound.x=(i+2)*tileWidth-1;
			queryArea.upperBound.y=(j+2)*tileHeight-1;
			callback.increment=30;
			g->getGSM()->getPhysics()->getWorld()->QueryAABB(&callback,queryArea);
			gridData[i][j]=(short)(obstacles);
		}
	}
}