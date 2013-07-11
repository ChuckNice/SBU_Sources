/**
 * 
 */
package minesweeper;


import java.util.Iterator;

import minesweeper.game.MinesweeperPP;
import minesweeper.level_editor.LevelEditorPP;
import mini_game.MiniGame;
import mini_game.MiniGameDataModel;
import mini_game.Sprite;
import mini_game.SpriteType;

import static minesweeper.MainMenu.*;


/**
 * @author Charlie
 *
 */

public class MainMenuDataModel extends MiniGameDataModel {

	/***************************************************************************
	 * 
	 **************************************************************************/
	public MainMenuDataModel() {
		// TODO Auto-generated constructor stub
	}

	
	 /**************************************************************************
     * Provides the appropriate game response to a player
     * pressing the mouse at (x,y) on the game canvas. Note
     * that the appropriate response may be to place a brain,
     * or collect sun, or do nothing.
     * 
     * @param game the currently active game
     * 
     * @param x the x-axis location of the mouse press
     * 
     * @param y the y-axis location of the mouse press
     **************************************************************************/
    @Override
    public void checkMousePressOnSprites(MiniGame game, int x, int y) {
        // WE'LL NEED Zombiquarium STUFF AS WELL
        MainMenu zGame = (MainMenu) game;
        SpriteType zT = spriteTypes.get(ZOMBIE_TYPE);
        int zW = zT.getWidth();
        int zH = zT.getHeight();

        // TRY TO PLACE A BRAIN AS LONG AS THERE ARE NOT ALREADY TOO MANY. NOTE
        // THAT WE WILL USE A SMALLER PLAYING AREA FOR PLACING BRAINS BECAUSE 
        // WE DON'T WANT TO PUT THEM WHERE ZOMBIES CAN'T REACH THEM
        // THE MAX AMOUNT ON SCREEN AT ANY TIME IS SPECIFIED IN ZOMBIQUARIUM
        // BRAINS SHOULD NOT BE PLACED IF THE GAME HAS NOT BEGUN OR IF PAUSED

        // PICKUP SUN
        
        // DID THE PLAYER CLICK ON A Game Square?
        if (!clickedOnGameSquare(x, y)) {

            //THEN TRY TO PLACE A BRAIN
            if ((brains.size() < MAX_BRAINS_ALLOWED)
                    && inProgress()
                    && !isPaused()) {
                // DOES THE PLAYER HAVE ENOUGH SUN TO BUY A BRAIN?
                if (collectedSun >= COST_BRAIN) {
                    // ADD THE BRAIN
                    if (((x >= (zGame.getBoundaryLeft() + zW))
                            && (x <= (zGame.getBoundaryRight() - zW))
                            && (y >= (zGame.getBoundaryTop() + zH))
                            && (y <= (zGame.getBoundaryBottom() - zH)))) {
                        SpriteType brainType = getSpriteType(BRAIN_TYPE);
                        Sprite newBrain = new Sprite(brainType, x, y, 0, .2f, DEFAULT_STATE);
                        addBrain(newBrain);

                        // SUBTRACT COST OF BRAIN FROM USERS STASH
                        collectedSun -= COST_BRAIN;
                    }
                }
            }
        }
    }
    
   
    /**************************************************************************
     * ME
     * clickedOnSun
     * This method returns true if the player clicked on a sun. 
     * @param x You know what this is.
     * @param y You know what this is.
     * @return if player clicked on a sun.
     * 
     **************************************************************************/
    public boolean clickedOnGameSquare(int x, int y){
        Iterator<Sprite> gameSquareIt = getGameSquareIterator();
        while (gameSquareIt.hasNext()) {
            Sprite sunPickupCandidate = sunIt.next();
            int sX = (int) sunPickupCandidate.getX();
            int sY = (int) sunPickupCandidate.getY();
            int sW = sunPickupCandidate.getSpriteType().getWidth();
            int sH = sunPickupCandidate.getSpriteType().getHeight();
            if ((x >= sX && x <= (sX + sW))
                    && (y >= sY && y <= sY + sH)) {
                markSunForDeletion(sunPickupCandidate);
                collectedSun += 25;
                return true;
            }
        }
        return false;
    }
    

	/* (non-Javadoc)
	 * @see mini_game.MiniGameDataModel#reset(mini_game.MiniGame)
	 */
	@Override
	public void reset(MiniGame game) {
		// TODO Auto-generated method stub

	}

	/* (non-Javadoc)
	 * @see mini_game.MiniGameDataModel#updateAll(mini_game.MiniGame)
	 */
	@Override
	public void updateAll(MiniGame game) {
		// TODO Auto-generated method stub

	}

	/* (non-Javadoc)
	 * @see mini_game.MiniGameDataModel#updateDebugText(mini_game.MiniGame)
	 */
	@Override
	public void updateDebugText(MiniGame game) {
		
		// WE DO NOT NEED DEBUG TEXT ON THE MAIN MENU SCREEN
		
	}
	
	/***************************************************************************
	 * 
	 **************************************************************************/
	public void startMinesweeper(){
		// TODO Auto-generated method stub
		MinesweeperPP game = new MinesweeperPP(GAME_APP_TITLE, FRAME_RATE);
		game.startGame();
	}

	/***************************************************************************
	 * 
	 **************************************************************************/
	public void startLevelEditor(){
		// TODO Auto-generated method stub
		LevelEditorPP editor = new LevelEditorPP(LEVEL_EDITOR_APP_TITLE, FRAME_RATE);
		editor.startGame();
	}
	
	/***************************************************************************
	 * 
	 **************************************************************************/
	public void startProfileEditor() {
		// TODO Auto-generated method stub
		
	}
	
	/***************************************************************************
	 * 
	 **************************************************************************/
	public void showHighScoresPanel(){
		// TODO Auto-generated method stub
		
	}
	
	/***************************************************************************
	 * 
	 **************************************************************************/
	public void showCreditsPanel(){
		// TODO Auto-generated method stub
		
	}
	


















	
}

// TODO REMOVE WHITESPACE BELOW

