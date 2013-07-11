#pragma once
#include <SSSF_SourceCode\thread\GameThread.h>
#include <SSSF_SourceCode\game\Game.h>
#include <process.h>
class GameThreads:public GameThread{
	
public:
	bool isDoneRendering,isDoneSwappingRenderList;
	HANDLE renderingMutex, swapLists;
	HANDLE renderThreadHandle;
	DWORD renderThreadID;
	GameThreads(){
		isDoneRendering=true;
		isDoneSwappingRenderList=false;//true;
		renderingMutex= CreateMutex(0,NULL,L"RenderingMutex");
		swapLists= CreateMutex(0,NULL,L"SwappingMutex");
	}
	virtual void kill();
	virtual void mainThread(Game* game);
	virtual void start(Game* game);
};