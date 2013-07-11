#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <SourceCode\ProjectBase\GameThreads.h>
/*
unsigned int _stdcall renderGame(void *args){
	Game* g = (Game*)args;
	GameThreads *thisObj= (GameThreads*)g->getThread();
	while(g->getGSM()->isAppActive()){
		if(thisObj->isDoneSwappingRenderList){
			WaitForSingleObject(thisObj->renderingMutex,INFINITE);
			//RENDER
			g->getGraphics()->renderGame(g);
			thisObj->isDoneRendering=true;
			ReleaseMutex(thisObj->renderingMutex);

			WaitForSingleObject(thisObj->swapLists,INFINITE);
			thisObj->isDoneSwappingRenderList=false;
			ReleaseMutex(thisObj->swapLists);
		}
	}
	return 0;
}

void GameThreads::mainThread(Game* game){
	if(isDoneRendering){
		game->processGameData();
		WaitForSingleObject(swapLists,INFINITE);
		WaitForSingleObject(renderingMutex,INFINITE);
		isDoneRendering=false;
		ReleaseMutex(renderingMutex);
		game->getGraphics()->swapRenderLists();
		this->isDoneSwappingRenderList=true;
		ReleaseMutex(swapLists);
	}
}
void GameThreads::kill(){

}

void GameThreads::start(Game* game){
	unsigned int id;
	renderThreadHandle=	(HANDLE)_beginthreadex(NULL,0,&renderGame,game,0,&id);
	renderThreadID=id;
}
*/
void GameThreads::mainThread(Game* game){
	game->processGameData();
	game->getGraphics()->renderGame(game);
	game->getGraphics()->swapRenderLists();
}
void GameThreads::kill(){

}

void GameThreads::start(Game* game){
}