#include <SourceCode\InGameCode\GUI\BarracksGUI.h>
#include <SourceCode\InGameCode\GUI\SpellToolbar.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <SSSF_SourceCode\graphics\RenderList.h>
#include <stdafx.h>
#include <SSSF_SourceCode\stdutil.h>
#include <SourceCode\InGameCode\GUI\SpellButton.h>
#include <SourceCode\InGameCode\Spells\Spell.h>
#include <SourceCode\ProjectBase\ResourceBank.h>
#include <SourceCode\InGameCode\Player.h>
#include <SourceCode\InGameCode\Units\Building\Barracks.h>

Barracks* BarracksGUI::currentBarracks=NULL;
void BarracksGUI::initialize(lua_State* s,Player* p){
	this->p=p;
	selected=-1;
	txtBox=NULL;
	currentlyEditing=-1;
	currentlyDisplaying=-1;
	popupOverlay = ResourceBankgetResource((string)"BarracksPopup");
	sliderImage = ResourceBankgetResource((string)"SliderBar");
	lineImage = ResourceBankgetResource((string)"LineImage");
	buildbtnImage =ResourceBankgetResource((string)"BuildButton");
	goldIcon = ResourceBankgetResource((string)"GoldIcon");
	popIcon = ResourceBankgetResource((string)"PopIcon");
	offX=luabind::object_cast<int>(luabind::globals(s)["BarracksGUILayout"]["offX"]);
	offY=luabind::object_cast<int>(luabind::globals(s)["BarracksGUILayout"]["offY"]);
	distBetweenRows=luabind::object_cast<int>(luabind::globals(s)["BarracksGUILayout"]["distBetweenRows"]);
	col1Space=luabind::object_cast<int>(luabind::globals(s)["BarracksGUILayout"]["col1Space"]);
	col2Space=luabind::object_cast<int>(luabind::globals(s)["BarracksGUILayout"]["col2Space"]);
	lineWidth=luabind::object_cast<int>(luabind::globals(s)["BarracksGUILayout"]["lineWidth"]);
	buildBtnX=luabind::object_cast<int>(luabind::globals(s)["BarracksGUILayout"]["buildBtnX"]);
	buildBtnY=luabind::object_cast<int>(luabind::globals(s)["BarracksGUILayout"]["buildBtnY"]);
	buildBtnW=luabind::object_cast<int>(luabind::globals(s)["BarracksGUILayout"]["buildBtnW"]);
	buildBtnH=luabind::object_cast<int>(luabind::globals(s)["BarracksGUILayout"]["buildBtnH"]);
	buttonScale=luabind::object_cast<float>(luabind::globals(s)["BarracksGUILayout"]["buttonScale"]);
	p->unitTypes.clear();
	for(luabind::iterator i(luabind::globals(s)["UnitEntriesDatabase"]),end;i!=end;++i){
		UnitEntry newType;
		int x=1;
		newType.BotRecyclerID=convertwstrX(luabind::object_cast<string>(i->operator[](x++)));
		newType.imageID=ResourceBankgetResource(luabind::object_cast<string>(i->operator[](x++)));
		newType.buildTime=luabind::object_cast<int>(i->operator[](x++));
		newType.popCost=luabind::object_cast<int>(i->operator[](x++));
		newType.goldCost=luabind::object_cast<int>(i->operator[](x++));
		newType.title=convertwstrX(luabind::object_cast<string>(i->operator[](x++)));
		newType.tooltip=convertwstrX(luabind::object_cast<string>(i->operator[](x++)));
		newType.titleFont=convertwstrX(luabind::object_cast<string>(i->operator[](x++)));
		newType.tooltipFont=convertwstrX(luabind::object_cast<string>(i->operator[](x++)));
		newType.width=luabind::object_cast<int>(i->operator[](x++));
		for(int k=0;k<4;k++)newType.titleColor[k]=luabind::object_cast<int>(i->operator[](x++));
		for(int k=0;k<4;k++)newType.tooltipColor[k]=luabind::object_cast<int>(i->operator[](x++));
		newType.numToBuild=0;
		newType.currentPercent=0;
		p->unitTypes.push_back(newType);
	}
	endSelf=false;
}
void BarracksGUI::render(RenderList* list){
	
}
void BarracksGUI::clearSelf(){
	vector<UnitEntry>::iterator begin = p->unitTypes.begin(),end=p->unitTypes.end();
	for(;begin!=end;begin++){
		p->gold+=begin->goldCost*begin->numToBuild;
		p->pop+=begin->popCost*begin->numToBuild;
		begin->numToBuild=0;
	}
}
void BarracksGUI::update(Game* g){
	if(endSelf){
		g->getGSM()->setOverlayGameState(0);
		endSelf=false;
	}
	if(txtBox==NULL && currentlyDisplaying!=-1){
		UnitEntry e = p->unitTypes[currentlyDisplaying];
		txtBox=g->createTextBoxSimple("","",offX+32,offY+distBetweenRows*currentlyDisplaying+32,e.width);
		g->setBG(txtBox,ResourceBankgetResource((string)"ToolTipBG"),10,10);
		g->setTitle(txtBox,convertWStr(e.title),convertWStr(e.titleFont),16,e.titleColor[0],e.titleColor[1],e.titleColor[2],e.titleColor[3]);
		g->setBody(txtBox,convertWStr(e.tooltip),convertWStr(e.tooltipFont),16,e.tooltipColor[0],e.tooltipColor[1],
			e.tooltipColor[2],e.tooltipColor[3]);
	}
}
void BarracksGUI::handleMouse(int x, int y){
	currentlyEditing=-1;
	if(currentlyDisplaying!=-1 && !(x>offX && x<offX+32 && y>offY+distBetweenRows*currentlyDisplaying && y< offY+distBetweenRows*currentlyDisplaying+32)){
		txtBox->release();
		txtBox=NULL;
		currentlyDisplaying=-1;
	}
	if(txtBox==NULL){
		vector<UnitEntry>::iterator begin = p->unitTypes.begin(),end=p->unitTypes.end();
		for(int i=0;begin!=end;begin++,i++){
			if(x>offX && x<offX+32 && y>offY+distBetweenRows*i && y< offY+distBetweenRows*i+32){
				currentlyDisplaying=i;
			}
		}
	}
}
void BarracksGUI::handleClick(int x, int y){
	
}
void BarracksGUI::handleClickDrag(int xi, int yi,int xf,int yf){
	if(currentlyEditing<0){
		vector<UnitEntry>::iterator begin = p->unitTypes.begin(),end=p->unitTypes.end();
		int ppop = p->tmpPop;
		int gold = p->tmpGold;
		for(int i=0;begin!=end;begin++,i++){
			int maxCapable = min((ppop+begin->numToBuild*begin->popCost)/begin->popCost,(gold+begin->numToBuild*begin->goldCost)/begin->goldCost);
			int cursorX=offX+col1Space+col2Space  +	(int)((float)begin->numToBuild/maxCapable*lineWidth);
			int cursorY=offY+distBetweenRows*i-13;
			if(xi>cursorX && xi<cursorX+32 && yi>cursorY && yi<cursorY+32){
				currentlyEditing=i;
			}
		}
	}else{
		int maxCapable = min((p->tmpPop+p->unitTypes[currentlyEditing].numToBuild*p->unitTypes[currentlyEditing].popCost)/p->unitTypes[currentlyEditing].popCost,(p->tmpGold+p->unitTypes[currentlyEditing].numToBuild*p->unitTypes[currentlyEditing].goldCost)/p->unitTypes[currentlyEditing].goldCost);
		int num = (int)(maxCapable*(float)(xf-16-offX-col1Space-col2Space)/lineWidth);
		if(num<0)
			num=0;
		else if (num>maxCapable)
			num=maxCapable;
		p->tmpPop-=(num-p->unitTypes[currentlyEditing].numToBuild)*p->unitTypes[currentlyEditing].popCost;
		p->tmpGold-=(num-p->unitTypes[currentlyEditing].numToBuild)*p->unitTypes[currentlyEditing].goldCost;
		p->unitTypes[currentlyEditing].numToBuild=num;
	}
}

void BarracksGUI::dispatchClickDrag(int xi, int yi,int xf,int yf){
	currentlyEditing=-1;
}
void BarracksGUI::onEnd(){
	if(txtBox!=NULL){
		txtBox->release();
	}
	clearSelf();
}
void BarracksGUI::onStart(){
	p->tmpPop=p->pop;
	p->tmpGold=p->gold;
}

void BarracksGUI::issueOrder(){
}