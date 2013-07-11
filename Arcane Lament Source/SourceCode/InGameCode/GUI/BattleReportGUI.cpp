#include <stdafx.h>
#include <SourceCode\InGameCode\GUI\BattleReportGUI.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SourceCode\ProjectBase\ResourceBank.h>
#include <SourceCode\InGameCode\InGameState.h>
#include <SSSF_SourceCode\graphics\RenderList.h>


BattleReportGUI::BattleReportGUI(void){
}


BattleReportGUI::~BattleReportGUI(void){
}

void BattleReportGUI::handleMouse(int x, int y){}
void BattleReportGUI::handleClick(int x, int y){}
void BattleReportGUI::handleClickDrag(int xi, int yi,int xf,int yf){}
void BattleReportGUI::dispatchClickDrag(int xi, int yi,int xf,int yf){}
void BattleReportGUI::render(RenderList* list){
	list->addRenderItem(popupOverlayID, 0, 0, 0, 255, 0, 0 );
	if( won == true ) {
		// DISPLAY THE VICTORY GRAPHIC ON TOP OF THE SCREEN
		int victoryID =  ResourceBankgetResource((string)"Victory");
		list->addRenderItem(victoryID, 200,100,0,255,100,100);
	}else{
		// DISPLAY THE GAME OVER GRAPHIC
		int defeatID =  ResourceBankgetResource((string)"Defeat");
		list->addRenderItem(defeatID, 200,100,0,255,100,100);
	}

	//RENDER STATS
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
	list->addRenderItem(130,335,245,146,0,255,&stats_left,&fontStrA,22);

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
	stats_right.append(stream.str());
	list->addRenderItem(415,335,0,128,255,255,&stats_right,&fontStrB,22);

	stream.str(L"");
	stats_right.clear();
	stream << "Total Archers Created:" << player->numArchersCreated << "\n";
	stream << "Total Archers Killed:" << player->numArchersKilled << "\n";
	stream << "Total Archers Lost:" << player->numArchersLost << "\n\n";
	stream << "Total Knights created:" << player->numKnightsCreated << "\n";
	stream << "Total Knights Killed:" << player->numKnightsKilled << "\n";
	stream << "Total Knights Lost:" << player->numKnightsLost << "\n";
	stats_right.append(stream.str());
	list->addRenderItem(690,335,0,128,255,255,&stats_right,&fontStrB,22);

}
void BattleReportGUI::update(Game* g){}

void BattleReportGUI::onStart(Game* g, bool won){

	game = g;
	iGs = (InGameState*)g->getGSM()->getManager();
	player = iGs->getPlayer();
	this->won = won;
	popupOverlayID =  ResourceBankgetResource((string)"BattleReportScreen");
	
}

void BattleReportGUI::onEnd(){}