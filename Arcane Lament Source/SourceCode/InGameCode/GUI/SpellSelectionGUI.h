#pragma once
#include <sourcecode\ingamecode\gui\guiscreenoverlay.h>
#include <SourceCode\InGameCode\GUI\SpellDBToolbar.h>
#include <SourceCode\InGameCode\GUI\SpellToolbar.h>
//#include <SourceCode\InGameCode\InGameState.h>

class RenderList;
class Game;
class SpellDB;
class Player;
class SpellToolbar;
class InGameState;
struct lua_State;
class SpellSelectionGUI : public GUIScreenOverlay{
	int popupOverlayID;
public:
	int x,y;
	SpellSelectionGUI();
	~SpellSelectionGUI();
	InGameState *iGs;
	Game *game;
	Player* player;
	SpellDBToolbar spDBToolbar;
	SpellToolbar spellToolbar;

	void createNewSpellButton(SpellButton* newSpellBtn);

	void setupScreen(Game *g, lua_State *state, SpellDB *spellDB);
	void handleMouse(int x, int y);
	void handleClick(int x, int y);
	void handleClickDrag(int xi, int yi,int xf,int yf);
	void dispatchClickDrag(int xi, int yi,int xf,int yf);
	void render(RenderList* list);
	void update(Game* g);
};

