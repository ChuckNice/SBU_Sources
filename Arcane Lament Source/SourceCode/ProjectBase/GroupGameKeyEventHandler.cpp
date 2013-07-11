/*
	Author: Angad Sidhu
*/

#include "stdafx.h"
#include "SourceCode\ProjectBase\GroupGameData.h"
#include "SourceCode\ProjectBase\GroupGameKeyEventHandler.h"
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\game\WStringTable.h"
#include "SSSF_SourceCode\graphics\GameGraphics.h"
#include "SSSF_SourceCode\gsm\physics\Physics.h"
#include "SSSF_SourceCode\gsm\sprite\AnimatedSprite.h"
#include "SSSF_SourceCode\gsm\state\GameState.h"
#include "SSSF_SourceCode\gsm\state\GameStateManager.h"
#include "SSSF_SourceCode\gui\Cursor.h"
#include "SSSF_SourceCode\gui\GameGUI.h"
#include "SSSF_SourceCode\input\GameInput.h"
#include "SSSF_SourceCode\timer\GameTimer.h"
#include <SSSF_SourceCode\PlatformPlugins\WindowsPlugin\WindowsInput.h>
#include "SSSF_SourceCode\PlatformPlugins\WindowsPlugin\WindowsTimer.h"
#include <SSSF_SourceCode\sound\SoundManager.h>
#include <SourceCode\InGameCode\InGameState.h>

// USED FOR MOVEMENT
const unsigned int ATTACK_KEY = (unsigned int)'Q';
const unsigned int MOVE_KEY = (unsigned int)'W';
const unsigned int PATROL_KEY = (unsigned int)'E';
const unsigned int STOP_KEY = (unsigned int)'R';
const unsigned int UP_KEY = VK_UP;
const unsigned int DOWN_KEY = VK_DOWN;
const unsigned int LEFT_KEY = VK_LEFT;
const unsigned int RIGHT_KEY = VK_RIGHT;

// THIS IS JUST FOR SHOWING HOW THE CURSOR CAN BE CHANGED
const unsigned int C_KEY = (unsigned int)'C';
const int MIN_FPS = 5;
const int MAX_FPS = 100;
const int FPS_INC = 2;

// THESE ARE FOR STORING XY TEMPORARILY TO DETERMINE IF THE USER 
// CLICKED AND DRAGGED OR JUST CLICKED


/*
	handleKeyEvent - this method handles all keyboard interactions. Note that every frame this method
	gets called and it can respond to key interactions in any custom way. Ask the GameInput class for
	key states since the last frame, which can allow us to respond to key presses, including when keys
	are held down for multiple frames.
*/
void GroupGameKeyEventHandler::handleKeyEvents(Game *game)
{
	// WE CAN QUERY INPUT TO SEE WHAT WAS PRESSED
	WindowsInput *input = (WindowsInput*)game->getInput();

	// LET'S GET THE PLAYER'S PHYSICAL PROPERTIES, IN CASE WE WANT TO CHANGE THEM
	GameStateManager *gsm = game->getGSM();
	
	if(gsm->isAtSplashScreen()&& input->isKeyDown(VK_RETURN))
		gsm->goToMainMenu();
	if(gsm->getCurrentGameState()==GS_GAME_IN_PROGRESS){
		if( input->isKeyDownForFirstTime(VK_ESCAPE) ){
			
			if(gsm->getManager()!=NULL && !((InGameState*)gsm->getManager())->useEscape()){
					game->clearTextBoxes();
					if(gsm->overlayGameState==GS_SPLASH_SCREEN){
						gsm->overlayGameState=GS_PAUSED;
						gsm->switchState();
					}
					else
						gsm->overlayGameState=GS_SPLASH_SCREEN;

			}
		}
	}
	if(gsm->getManager()!=NULL){
		InGameState *state = (InGameState*)gsm->getManager();
		if(!mousePrevPressed)
			state->handleMouseInput(input->getMouseX(),input->getMouseY());
		if(input->isKeyDownForFirstTime(VK_RBUTTON))
			state->handleRightClickInput(input->getVPMouseX(),input->getVPMouseY(),input->getMouseX(),input->getMouseY());
		// TRACK MOUSE AND INITIAL XY
		if (input->isKeyDown( VK_LBUTTON ) && !mousePrevPressed){
			xi = input->getVPMouseX();
			yi = input->getVPMouseY();
			screenxi=input->getMouseX();
			screenyi=input->getMouseY();
			mousePrevPressed = true;
		}

		if(input->isKeyDownForFirstTime('Q'))
			state->mainPlayer->queueUnit(0,0,3);
		if(input->isKeyDownForFirstTime('W'))
			state->mainPlayer->queueUnit(1,0,3);
		if(input->isKeyDownForFirstTime('E'))
			state->mainPlayer->queueUnit(2,0,3);
		if(input->isKeyDownForFirstTime('R'))
			state->mainPlayer->queueUnit(3,0,3);
		if(input->isKeyDownForFirstTime('T'))
			state->mainPlayer->queueUnit(4,0,3);

		/////////////////////////////////// CHEATS ////////////////////////////////////////////
		//SPAWN BOTH OUR UNITS
		if(input->isKeyDown(VK_CONTROL) && input->isKeyDownForFirstTime('K')){
			//OURS
			for(int i=0;i<2;i++){
				Unit *u =(Unit*)gsm->getSpriteManager()->getBotRecycler()->retrieveBot(L"Knight");
				state->addUnit(u,state->players[0]->tower->getX()+rand()%100-50,
					state->players[0]->tower->getY()+rand()%100-50,state->players[0]);
				u->issueAttackToOrder(state->players[1]->tower->getWorldX(),state->players[1]->tower->getWorldY());
			}
			for(int i=0;i<10;i++){
				Unit *u =(Unit*)gsm->getSpriteManager()->getBotRecycler()->retrieveBot(L"WARRIOR");
				state->addUnit(u,state->players[0]->tower->getX()+rand()%100-50,
					state->players[0]->tower->getY()+rand()%100-50,state->players[0]);
				u->issueAttackToOrder(state->players[1]->tower->getWorldX(),state->players[1]->tower->getWorldY());
			}
			//THEIRS
			for(int i=0;i<10;i++){
				Unit *u =(Unit*)gsm->getSpriteManager()->getBotRecycler()->retrieveBot(L"WARRIOR");
				state->addUnit(u,state->players[1]->tower->getX()+rand()%100-50,
					state->players[1]->tower->getY()+rand()%100-50,state->players[1]);
				u->issueAttackToOrder(state->players[0]->tower->getWorldX(),state->players[0]->tower->getWorldY());
			}
			for(int i=0;i<1;i++){
				Unit *u =(Unit*)gsm->getSpriteManager()->getBotRecycler()->retrieveBot(L"OGRE");
				state->addUnit(u,state->players[1]->tower->getX()+rand()%100-50,
					state->players[1]->tower->getY()+rand()%100-50,state->players[1]);
				u->issueAttackToOrder(state->players[0]->tower->getWorldX(),state->players[0]->tower->getWorldY());
			}
			for(int i=0;i<2;i++){
				Unit *u =(Unit*)gsm->getSpriteManager()->getBotRecycler()->retrieveBot(L"Wizard");
				state->addUnit(u,state->players[1]->tower->getX()+rand()%100-50,
					state->players[1]->tower->getY()+rand()%100-50,state->players[1]);
				u->issueAttackToOrder(state->players[0]->tower->getWorldX(),state->players[0]->tower->getWorldY());
			}
		}

		// KILL OPPONENT TOWER 
		if (input->isKeyDown(VK_CONTROL) && input->isKeyDownForFirstTime('M')) {
			((InGameState*)game->getGSM()->getManager())->players[1]->tower->damageUnit(500.0f);
		}
		// KILL OPPONENT TOWER
		if (input->isKeyDown(VK_CONTROL) && input->isKeyDownForFirstTime('N')) {
			((InGameState*)game->getGSM()->getManager())->players[0]->tower->damageUnit(500.0f);
		}
		// GET MONEY!!!... OH, AND MANA
		if (input->isKeyDown(VK_CONTROL) && input->isKeyDownForFirstTime('L')) {
			((InGameState*)game->getGSM()->getManager())->players[0]->gold+=500;
			((InGameState*)game->getGSM()->getManager())->players[0]->mana=((InGameState*)game->getGSM()->getManager())->players[0]->maxmana ;
		}

		/////////////////////////////////// END CHEATS ////////////////////////////////////////////

		// SELECT UNITS IF SELECT IS HELD
		if( mousePrevPressed && input->isKeyDown( VK_LBUTTON ) ){
			xf = input->getMouseX();
			yf = input->getMouseY();
			//if ( (abs(xf - screenxi) > 30 ) || ( abs(yf - screenyi) > 30) ){
				state->handleClickDragInput( xi, yi, xf, yf,screenxi,screenyi);
			//}
		}

		// GET FINAL XY
		if( mousePrevPressed && !input->isKeyDown( VK_LBUTTON )){
			xf = input->getMouseX();
			yf = input->getMouseY();

			// CHECK TOLERANCE FOR CLICK+DRAG
			if ( (abs(xf - screenxi) > 30 ) || ( abs(yf - screenyi) > 30) ){
				state->dispatchClickDragInput( xi, yi, xf, yf,screenxi,screenyi);
			}else{
				state->handleClickInput( xi, yi,screenxi,screenyi);
			}
			mousePrevPressed = false;
		}	

	}


	// IF THE GAME IS IN PROGRESS
	if (gsm->isGameInProgress() && gsm->getManager()!=NULL)
	{
		//Update for game in progress
		InGameState *state = (InGameState*)gsm->getManager();
		Cursor *c = game->getGUI()->getCursor(GS_GAME_IN_PROGRESS);
		Viewport *vp = game->getGUI()->getViewport();
		if(input->isKeyDownForFirstTime('1')){
				state->toolbar.setDown(0);
				//game->getSoundManager()->playCue("VICTORY");
			}else if(input->isKeyDownForFirstTime('2')){
				state->toolbar.setDown(1);
				//game->getSoundManager()->playCue("VICTORY");
			}else if(input->isKeyDownForFirstTime('3')){
				state->toolbar.setDown(2);
				//game->getSoundManager()->playCue("VICTORY");
			}else if(input->isKeyDownForFirstTime('4')){
				state->toolbar.setDown(3);
				//game->getSoundManager()->playCue("VICTORY");
			}
			else if(input->isKeyDownForFirstTime('5')){
				state->toolbar.setDown(4);
				//game->getSoundManager()->playCue("VICTORY");
			}
		//if(input->isKeyDownForFirstTime('Z')){
		//		state->toolbar.setDown(4);
		//		//game->getSoundManager()->playCue("VICTORY");
		//	}else if(input->isKeyDownForFirstTime('X')){
		//		state->toolbar.setDown(5);
		//		//game->getSoundManager()->playCue("VICTORY");
		//	}else if(input->isKeyDownForFirstTime('C')){
		//		state->toolbar.setDown(6);
		//		//game->getSoundManager()->playCue("VICTORY");
		//	}else if(input->isKeyDownForFirstTime('V')){
		//		state->toolbar.setDown(7);
		//		//game->getSoundManager()->playCue("VICTORY");
		//	}
		//Dispatch key events for hot keys

		// IF NOT HOLDING SHIFT THE USER IS WANTING TO CAST A SPELL
		// AND THE NEXT CLICK WILL CAST OR BEGIN CASTING BASED ON THE
		// TYPE OF SPELL
		if( input->isKeyDown(VK_SHIFT) ){	
			state->setShiftDown( true );
		}else{
			state->setShiftDown( false );
		}
		
		// SPACE HELD DOWN MEANS THAT THE MOUSE MOVEMENT WILL 
		// MOVE THE VIEWPORT
		//if (input->isKeyDown(VK_SPACE)){
		//	game->getGUI()->getViewport()->setHaltUpdate( true );//true
		//}else{
		//	game->getGUI()->getViewport()->setHaltUpdate( false );//false
		//}

		


	}
}