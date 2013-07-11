#include <stdafx.h>
#include <SourceCode\InGameCode\GUI\SpellDBToolbar.h>
#include <SSSF_SourceCode\text\TextBoxToDraw.h>
#include <SourceCode\InGameCode\SpellDB.h>
#include <SourceCode\InGameCode\InGameState.h>
#include <SSSF_SourceCode\graphics\RenderList.h>
#include <SSSF_SourceCode\game\Game.h>
#include <SSSF_SourceCode\sound\SoundManager.h>
#include<SourceCode\WizardSystem\Wizard.h>

void SpellDBToolbar::initializeToolbar(InGameState* iGs, SpellDB *spellDB){
	//InGameState *iGs= (InGameState*)g->getGSM()->getManager();
	//SpellDB *sDB = iGs->getSpellDB();
	btns.clear();
	state = iGs;
	int toolbarCol = 0;
	int toolbarRow = 0;
	multimap<SpellClass, SpellButton*> buttonMap;
	map<string, SpellAndButtonPair>::iterator itA = spellDB->getDbIterator(0);
	multimap<SpellClass, SpellButton* >::iterator itB;

	for (int i = 0; i <spellDB->getSpellCount(); i++, itA++){

		//PREPARE FOR DISPLAY WITH BLANK SPACES
		if((*itA).first != "ButtonSocket"){
			buttonMap.insert(make_pair((*itA).second.second->getSpellClass(), (*itA).second.first));
		}
	}
	int i=0;
	//DISPLAY THE BUTTONS WITH BLANKS AS NECESSARY
	for(itB = buttonMap.begin(); itB != buttonMap.end() && toolbarRow<8;){
		if((*itB).first == toolbarRow){
			map<string, int>::iterator it = state->getPlayer()->getWizard()
				->getUnlockedSpells().find(itB->second->title);
			if(it!=state->getPlayer()->getWizard()
									->getUnlockedSpells().end())
				btns.push_back( (*itB).second );
			else
				btns.push_back( spellDB->getSpellButton("ButtonSocket") );
			toolbarCol++;
			i++;
			itB++;
		}else{
			while(toolbarCol==0 || toolbarCol%5 != 0){
				btns.push_back( spellDB->getSpellButton("ButtonSocket") );
				toolbarCol++;
				i++;
			}
				toolbarCol = 0;
				toolbarRow++;
		}

			/*if ( (*itB).first == toolbarRow)
				itB++;*/

	}
	
	while(toolbarRow++<8){
			while(toolbarCol==0 || toolbarCol%5 != 0){
				btns.push_back( spellDB->getSpellButton("ButtonSocket") );
				toolbarCol++;
			}
			toolbarCol = 0;
		}

}


void SpellDBToolbar::handleMouse(InGameState* state,Game* g,int mx, int my){
	if(mx<x||mx>x+w||my<y||my>y+h){
		hotPos=-1;
		if(textBox!=NULL){
			textBox->release();
			textBox=NULL;
		}
		return;
	}
	int dx = mx-x;
	int dy = my-y;
	dx/=btnWidth;
	dy/=btnHeight;
	int pos =(dy*(w/btnWidth)+dx);
	if(pos<btns.size()){
		if(hotPos!=pos){
			hotPos=pos;
			hotTimer=0;
			if(textBox!=NULL){
				textBox->release();
				textBox=NULL;
			}
			SpellButton *b = btns.at(pos);
			textBox=g->createTextBoxSimple(b->title,"",mx,my-40,200);
			g->setTitle(textBox,b->title,b->titleFont,18,b->titleColor[0],b->titleColor[1],b->titleColor[2],b->titleColor[3]);
			g->setBG(textBox,b->backgroundID,10,5);
		}
		else hotTimer++;
		if(hotTimer==24){
			textBox->release();
			SpellButton *b = btns.at(pos);
			textBox=g->createTextBoxSimple(b->title,b->tooltip,textBox->x,textBox->y-60,200);
			g->setTitle(textBox,b->title,b->titleFont,18,b->titleColor[0],b->titleColor[1],b->titleColor[2],b->titleColor[3]);
			g->setBody(textBox,b->tooltip,b->tooltipFont,16,b->tooltipColor[0],b->tooltipColor[1],b->tooltipColor[2],b->tooltipColor[3]);
			g->setBG(textBox,b->backgroundID,10,5);
		}
	}
	else{ 
		hotPos=-1;
		if(textBox!=NULL){
			textBox->release();
			textBox=NULL;
		}
	}
}


void SpellDBToolbar::render(RenderList* list){
	int wBtns = w/btnWidth;
	for(int i=0;i<btns.size();i++){

		SpellButton *b= btns.at(i);

		//GET THE WIZARDS UNLOCKED SPELLS AND RENDER AS DOWN
		
		map<string, int>::iterator it = state->getPlayer()->getWizard()
									->getUnlockedSpells().find(btns.at(i)->title);
		
		if(it!=state->getPlayer()->getWizard()->getUnlockedSpells().end())
			list->addRenderItem(b->upImg,
			x+(i%wBtns)*btnWidth,
			y+i/wBtns*btnHeight,
			0,
			255,
			btnWidth,
			btnHeight
			);
		else list->addRenderItem(b->upImg,
			x+(i%wBtns)*btnWidth,
			y+i/wBtns*btnHeight,
			0,
			255,
			btnWidth,
			btnHeight
			);
	}
}


void SpellDBToolbar::handleClick(InGameState* state,Game* g, int mx, int my){
	if(mx<x||mx>x+w||my<y||my>y+h)return;
	int dx = mx-x;
	int dy = my-y;
	dx/=btnWidth;
	dy/=btnHeight;
	int pos =(dy*(w/btnWidth)+dx);
		
		
	if(pos<btns.size()){
		if(btns.at(pos)->title!="ButtonSocket")
				currentSpell=btns.at(pos)->title;
		//PLAY cue
		//state->createNewSpellButton((SpellButton*)btns.at(pos).first);
	}
}