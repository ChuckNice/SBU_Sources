/**
 * 
 */
package minesweeper.events;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;

import minesweeper.MainMenu;
import minesweeper.MainMenuDataModel;
import mini_game.Sprite;

/**
 * @author Charlie
 *
 */
public class MainMenuButtonHandler implements ActionListener{
	// HERE'S THE GAME WE'LL UPDATE
	private MainMenu game;

	/**
	 * This constructor just stores the game for later.
	 * 
	 * @param initGame the game to update
	 */
	public MainMenuButtonHandler(MainMenu initGame)
	{
		game = initGame;
	}

	/**
	 * Here is the event response. This code is executed when
	 * the user clicks on the button for starting a new game,
	 * which can be done when the application starts up, during
	 * a game, or after a game has been played. Note that the game 
	 * data is already locked for this thread before it is called, 
	 * and that it will be unlocked after it returns.
	 * 
	 * @param ae the event object for the button press
	 */
	@Override
	public void actionPerformed(ActionEvent ae){
		// GET THE DATA SO WE CAN CHANGE IT
		MainMenuDataModel data = (MainMenuDataModel) game.getDataModel();

		// IDENTIFY THE SOURCE BUTTON AND THE CURRENT STATE OF THE BUTTON
		Sprite source = (Sprite) ae.getSource();
		String buttonType = source.getSpriteType().getSpriteTypeID();

		// IS THE PRESSED BUTTON ENABLED?
		if (source.getState().equals(MainMenu.ENABLED_STATE)) {
			// WHICH GUI BUTTON WAS PRESSED?  
			// WAS IT THE NEW GAME BUTTON?
			if (buttonType.equals(MainMenu.NEW_GAME_TYPE)) {
				data.startMinesweeper();
			}// WAS IT THE LEVEL EDITOR BUTTON
			else if (buttonType.equals(MainMenu.LEVEL_EDITOR_TYPE)) {
				data.startLevelEditor();
			}
		}
	}
	// RESET EVERYTHING FOR A NEW GAME
//	game.reset();


}
