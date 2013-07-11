#pragma once
#include <SourceCode\InGameCode\GUI\GUIScreenOverlay.h>

class RenderList;
class Game;
class Player;
class InGameState;
struct lua_State;
class BattleReportGUI :public GUIScreenOverlay{
	int popupOverlayID;
public:

	InGameState *iGs;
	Game *game;
	Player* player;
	bool won;
	wstring fontStrA, fontStrB;
	wstring stats_left, stats_right;

	BattleReportGUI(void);
	~BattleReportGUI(void);

	void handleMouse(int x, int y);
	void handleClick(int x, int y);
	void handleClickDrag(int xi, int yi,int xf,int yf);
	void dispatchClickDrag(int xi, int yi,int xf,int yf);
	void render(RenderList* list);
	void update(Game* g);
	void onStart(Game* g, bool won);
	void onEnd();
};

