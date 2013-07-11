#include <SourceCode\InGameCode\GUI\TutorialSysGUI.h>
#include <SourceCode\ProjectBase\ResourceBank.h>
#include <SSSF_SourceCode\graphics\RenderList.h>
#include <SSSF_SourceCode\text\TextBoxToDraw.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\gsm\state\GameStateManager.h>
#include <luabind\luabind.hpp>
extern "C"{
#include "lua.h"
}
void TutorialSysGUI::setupSys(InGameState* s,Game *g, lua_State *st){
	this->iGs=s;
	this->game=g;
	background = game->getResManager()->getImageIDLua("$$BlackAlphaScreen",true);
	state= luaL_newstate();
	luabind::open(state);
	//string toolTipBG = "ToolTipBG";
}
void TutorialSysGUI::loadScreen(){
	vector<TextBoxToDraw*>::iterator begin=toRelease.begin(),
		end=toRelease.end();
	for(;begin!=end;begin++)
		(*begin)->release();
	toRelease.clear();
	images.clear();	
	textBoxes.clear();
	//luabind::object_cast<int>
	luaL_dofile(state,((string)"data/lua/tutorials/TutorialSys.lua").c_str());
	luabind::object db=luabind::globals(state)["TutorialDatabase"][currentTutorial];
	luabind::object screenReel = db[currentScreen];
	if(luabind::type(screenReel)==LUA_TNIL)
		game->getGSM()->overlayGameState=0;//end
	else{
		for(luabind::iterator i(screenReel),end;i!=end;i++){
			luabind::object a = *i;
			string imageID=luabind::object_cast<string>(a[1]);
			if(imageID!=""){
				TutorialSysImage img;
				img.imgId= game->getResManager()->getImageIDLua(imageID.c_str(),true);
				img.x=luabind::object_cast<int>(a[2]);
				img.y=luabind::object_cast<int>(a[3]);
				this->images.push_back(img);
			}
			int x, y,w;
			x=luabind::object_cast<int>(a[4]);
			y=luabind::object_cast<int>(a[5]);
			w=luabind::object_cast<int>(a[6]);
			string txt = luabind::object_cast<string>(a[7]);
			if(txt!=""){
				TutorialSysText TStxt;
				TStxt.text=txt;
				TStxt.x=x;
				TStxt.y=y;
				TStxt.width=w;
				TStxt.font="Calibri";
				TStxt.size=24;
				this->textBoxes.push_back(TStxt);
				TextBoxToDraw* draw=game->createTextBoxSimple("","",x,y,w);
				game->setBody(draw,txt,TStxt.font,TStxt.size,255,20,230,255);

				string toolTipBG = "ToolTipBG";
				int bgID = ResourceBankgetResource(toolTipBG);
				game->setBG(draw,bgID,10,5);
				this->toRelease.push_back(draw);
			}
		}
	}

}
void TutorialSysGUI::render(RenderList* list){	
	list->addRenderItem(background, 0, 0, 0, 255, 0, 0 );
	vector<TutorialSysImage>::iterator begin = images.begin(),
		end= images.end();
	for(;begin!=end;begin++)
		list->addRenderItem(begin->imgId,begin->x,begin->y,0,255,0,0);
	
}
void TutorialSysGUI::onEnd(){
	vector<TextBoxToDraw*>::iterator begin=toRelease.begin(),
		end=toRelease.end();
	for(;begin!=end;begin++)
		(*begin)->release();
	toRelease.clear();
}