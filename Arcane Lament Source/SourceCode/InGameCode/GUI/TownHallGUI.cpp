#include <SourceCode\InGameCode\GUI\TownHallGUI.h>
#include <SourceCode\InGameCode\GUI\SpellToolbar.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\graphics\RenderList.h>
#include <stdafx.h>
#include <SourceCode\InGameCode\GUI\SpellButton.h>
#include <SourceCode\InGameCode\Spells\Spell.h>
#include <SourceCode\ProjectBase\ResourceBank.h>
#include <SourceCode\InGameCode\Player.h>
void TownHallGUI::setupForPlayer(Player* p){
	this->p=p;
	popupOverlay = ResourceBankgetResource((string)"ProductionSettingsPopup");
	sliderImage = ResourceBankgetResource((string)"SliderBar");
	dragSliderOne=dragSliderTwo=false;
	sliderOneX=250;
	sliderOneY=260;
	sliderTwoX=250;
	sliderTwoY=410;
	slidersMin=240;
	slidersMax=590;
	tstTxt=L"HELLO";
	tstFont=L"Ariel";
}
void TownHallGUI::render(RenderList* list){
	list->addRenderItem(popupOverlay,0,0,0,255,0,0);
	list->addRenderItem(sliderImage,sliderOneX,sliderOneY,0,255,0,0);
	list->addRenderItem(sliderImage,sliderTwoX,sliderTwoY,0,255,0,0);
	tstTxt.clear();
	wstringstream test;
	test<<p->goldIncome<<L"/"<<(float)(int)(10*p->popIncome)/10;
	tstTxt.append(test.str());
	list->addRenderItem(630,260,255,255,255,255,&tstTxt,&tstFont,12);
}
void TownHallGUI::update(Game* g){
	if(!dragSliderOne){
		sliderOneX =p->economyVsGrowthFactor*(slidersMax-slidersMin)+slidersMin;
	}
}
void TownHallGUI::handleMouse(int x, int y){
	dragSliderOne=dragSliderTwo=false;
}
void TownHallGUI::handleClick(int x, int y){
	
}
void TownHallGUI::handleClickDrag(int xi, int yi,int xf,int yf){
	if(xi>sliderOneX && xi<sliderOneX+32 && yi>sliderOneY && yi<sliderOneY+32){
		dragSliderOne=true;
	}
	if(dragSliderOne){
		if(xf<slidersMin)sliderOneX=slidersMin;
		else if(xf>slidersMax)sliderOneX=slidersMax;
		else sliderOneX=xf;
		float percent = ((float)sliderOneX-slidersMin)/(slidersMax-slidersMin);
		p->economyVsGrowthFactor=percent;
		p->goldIncome=15+p->pop*p->goldPerPOP*percent;
		p->popIncome=p->pop*p->pgrPerPOP*(1-percent);
	}
}
void TownHallGUI::dispatchClickDrag(int xi, int yi,int xf,int yf){
	dragSliderOne=dragSliderTwo=false;
}