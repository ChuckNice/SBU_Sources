#include <stdafx.h>
#include "StatsScreen.h"
#include <SSSF_SourceCode\game\Game.h>
#include <SourceCode\ProjectBase\ResourceBank.h>
#include <SourceCode\InGameCode\InGameState.h>
#include <SSSF_SourceCode\graphics\RenderList.h>
#include <SourceCode\InGameCode\GUI\TutorialSysGUI.h>

StatsScreen::StatsScreen(void)
{
	fontStrA=L"Fantasy";
	fontStrB=L"Fantasy";
}


StatsScreen::~StatsScreen(void)
{
}


void StatsScreen::handleMouse(int x, int y){}
void StatsScreen::handleClick(int x, int y){}
void StatsScreen::handleClickDrag(int xi, int yi,int xf,int yf){}
void StatsScreen::dispatchClickDrag(int xi, int yi,int xf,int yf){}
void StatsScreen::update(Game* g){}


void StatsScreen::render(RenderList* list){

	list->addRenderItem(popupOverlayID, 0, 0, 0, 255, 0, 0 );
	stats_left.clear();
	stats_right.clear();

	wstringstream stream;
	stream << "Total Units Created:" << player->numCreated << "\n";
	stream << "Total Spells Cast:" << player->numSpells << "\n";
	stream << "Total Gold Accrued:" << player->goldAcrued << "\n";
	stream << "Total Units Killed:" << player->numKills << "\n";
	stream << "Total Units Lost:" << player->numLost << "\n";
	stream << "Total Mana Spent:" << player->manaUsed << "\n";
	stats_left.append(stream.str());
	list->addRenderItem(180,215,245,146,0,255,&stats_left,&fontStrA,22);

	stream.str(L"");

	stream << "Total Warriors Created:" << player->numWarriorsCreated << "\n";
	stream << "Total Warriors Killed:" << player->numWarriorsKilled << "\n";
	stream << "Total Warriors Lost:" << player->numWarriorsLost << "\n\n";
	stream << "Total Wizards Created:" << player->numWizardsCreated << "\n";
	stream << "Total Wizards Killed:" << player->numWizardsKilled << "\n";
	stream << "Total Wizards Lost:" << player->numWizardsLost << "\n\n";
	stream << "Total Ogres Created:" << player->numOgresCreated << "\n";
	stream << "Total Ogres Killed:" << player->numOgresKilled << "\n";
	stream << "Total Ogres Lost:" << player->numOgresLost << "\n\n";
	stream << "Total Archers Created:" << player->numArchersCreated << "\n";
	stream << "Total Archers Killed:" << player->numArchersKilled << "\n";
	stream << "Total Archers Lost:" << player->numArchersLost << "\n\n";
	stream << "Total Knights created:" << player->numKnightsCreated << "\n";
	stream << "Total Knights Killed:" << player->numKnightsKilled << "\n";
	stream << "Total Knights Lost:" << player->numKnightsLost << "\n";
	stats_right.append(stream.str());
	list->addRenderItem(460,215,0,128,255,255,&stats_right,&fontStrB,22);
}

void StatsScreen::onStart(Game* g){

	game = g;
	iGs = (InGameState*)g->getGSM()->getManager();
	player = iGs->getPlayer();
	popupOverlayID =  ResourceBankgetResource((string)"InGameStatsScreen");
	
}

void StatsScreen::onEnd(){
	
	/*if(!iGs->mainPlayer->wizard->startGameTutorial){
			game->getGSM()->overlayGameState=TUTSYSSCREENOVERLAYNUM;
			iGs->tutSys->currentTutorial="StartGame";
			iGs->mainPlayer->wizard->startGameTutorial=true;
		}*/
}