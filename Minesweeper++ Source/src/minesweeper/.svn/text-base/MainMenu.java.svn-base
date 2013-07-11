package minesweeper;

import java.awt.Color;
import java.awt.Font;
import java.awt.Image;
import java.io.BufferedReader;
import java.io.FileReader;

import javax.swing.JOptionPane;

import minesweeper.events.MainMenuButtonHandler;
import mini_game.MiniGame;
import mini_game.Sprite;
import mini_game.SpriteType;


public class MainMenu extends MiniGame {

	// THESE CONSTANTS SETUP THE GAME DIMENSIONS. THE GAME WIDTH
    // AND HEIGHT SHOULD MIRROR THE BACKGROUND IMAGE DIMENSIONS. WE
    // WILL NOT RENDER ANYTHING OUTSIDE THOSE BOUNDS. THE BOUNDARY
    // VALUES REFER TO AN INVISIBLE, PLAYABLE REGION. WE WON'T LET
    // THE ZOMBIES GO OUTSIDE THAT REGION AND WE WON'T HANDLE USER
    // MOUSE CLICKS THERE. NOTE THAT WE USE REAL NUMBERS FOR THE
    // BOUNDARY VALUES, WHICH REPRESENT PERCENTAGES. SO 0.2f FOR THE
    // TOP BOUNDARY WOULD MEAN 20% FROM THE TOP OF THE CANVAS
    public static final int GAME_WIDTH = 800;
    public static final int GAME_HEIGHT = 600;
    public static final float BOUNDARY_TOP = 0.1f;
    public static final float BOUNDARY_BOTTOM = 0.05f;
    public static final float BOUNDARY_LEFT = 0.01f;
    public static final float BOUNDARY_RIGHT = 0.01f;
    
    // HERE IS ALL THE MinesweeperPP SETUP VALUES. NOTE THAT
    // A GOOD ALTERNATIVE APPROACH WOULD BE TO LOAD THESE
    // VALUES FROM A FILE, WHICH WOULD LET A GAME DESIGNER
    // EDIT THEM AND TEST CHANGES WITHOUT HAVING THE WRITE
    // SOURCE CODE, WHICH IS GOOD, BECAUSE AS PROGRAMMERS
    // WE DON'T WANT NON-PROGRAMMERS MANGLING OUR ART. THAT'S
    // THE WHOLE POINT OF SCRIPTING LANGUAGES. NOTE THAT I 
    // STRONGLY ADVISE YOU TO CHANGE THESE VALUES DURING 
    // TESTING DEPENDING ON WHAT YOU ARE DOING TO MAKE YOUR 
    // LIFE EASIER, THEY SHOULD BE RETURNED TO THEIR PROPER 
    // VALUES UPON SUBMISSION.
    
    public static final float ZOMBIE_MIN_VELOCITY = 0.7f;
    public static final float ZOMBIE_MAX_VELOCITY = 3.0f;
    public static final int DEAD_ON_DISPLAY_TIME = 50;       
    public static final int MAX_BRAINS_ALLOWED = 3;            
    public static final float BRAIN_FALL_VELOCITY = 0.3f;
    
    // HERE ARE SOME APP-LEVEL SETTINGS, LIKE THE FRAME RATE. ALSO,
    // WE WILL BE LOADING SpriteType DATA FROM A FILE, SO THAT FILE
    // LOCATION IS PROVIDED HERE AS WELL. NOTE THAT IT MIGHT BE A 
    // GOOD IDEA TO LOAD ALL OF THESE SETTINGS FROM A FILE, BUT ALAS,
    // THERE ARE ONLY SO MANY HOURS IN A DAY
    public static final int FRAME_RATE = 30;
    public static final String APP_TITLE = 					"Minesweeper++ Main Menu";
    public static final String GAME_APP_TITLE = 			"Minesweeper++";
    public static final String LEVEL_EDITOR_APP_TITLE =		"Minesweeper++ Level Editor";
    public static final String PROFILE_EDITOR_APP_TITLE = 	"Minesweeper++ Profile Editor";
    
    // THESE ARE THE SETUP FILES FOR THE SPRITE TYPES  THAT WILL LOAD 
    // THINGS SUCH AS THE SPRITE TYPES AND STATES AND THE IMAGES ASSOCIATED 
    // WITH THOSE SPRITE TYPES
    public static final String MAIN_MENU_SPRITE_TYPES_SETUP_FILE = "./setup/M++_MainMenu_SetupData.txt";
    public static final String MAIN_GAME_SPRITE_TYPES_SETUP_FILE = "./setup/M++_MainGame_SetupData.txt";
    public static final String LEVEL_EDITOR_SPRITE_TYPES_SETUP_FILE = "./setup/M++_LevelEditor_SetupData.txt";
    public static final String PROFILE_EDITOR_SPRITE_TYPES_SETUP_FILE = "./setup/M++_ProfileEditor_SetupData.txt";
    public static final String SETUP_DELIMITER = 	"\\|";
    
    // COLOR KEYS THAT FACILITATE THE LOADING OF SPRITES WITH
    // TRANSPARENT BACKGROUNDS
    public static final Color COLOR_KEY_ORANGE = 	new Color(220, 110, 0);
    public static final Color COLOR_KEY_WHITE = 	new Color(255, 255, 255);
    
    // WE'LL USE THESE JUST TO MAKE SURE WE'RE CONSISTENT
    // WE'LL USE THESE FOR IDENTIFYING SPRITE TYPES WHEN WE NEED TO GET THEM
    public static final String BACKGROUND_TYPE = 	"BACKGROUND_TYPE";
    public static final String BRAIN_TYPE = 		"BRAIN_TYPE";
    public static final String ZOMBIE_TYPE = 		"ZOMBIE_TYPE";
    public static final String GAME_SQUARE_TYPE = 	"GAME_SQUARE_TYPE";
    public static final String MINE_TYPE = 			"MINE_TYPE";
        
    // AND THESE FOR IDENTIFYING CONTROL STATES WHEN
    // WE NEED TO EITHER TEST OR CHANGE THEM
    public static final String ENABLED_STATE = 		"ENABLED_STATE";
    public static final String DISABLED_STATE = 	"DIABLED_STATE";
    public static final String DEFAULT_STATE = 		"DEFAULT_STATE";
    public static final String MOUSE_OVER_STATE = 	"MOUSE_OVER_STATE";
    public static final String INVISIBLE_STATE = 	"INVISIBLE_STATE";
    public static final String VISIBLE_STATE = 		"VISIBLE_STATE";
    public static final String WON_GAME_STATE = 	"WON_STATE";
    public static final String LOSS_GAME_STATE =	"LOSS_STATE";

    // THESE ARE SPRITE SPECIFIC STATES USED FOR TESTING AND MANIPULATION
    // ==ZOMBIE==
    public static final String NORMAL_ZOMBIE_LEFT_STATE = 	"NORMAL_ZOMBIE_LEFT_STATE";
    public static final String NORMAL_ZOMBIE_RIGHT_STATE = 	"NORMAL_ZOMBIE_RIGHT_STATE";
    public static final String NORMAL_ZOMBIE_UP_STATE = 	"NORMAL_ZOMBIE_UP_STATE";
    public static final String NORMAL_ZOMBIE_DOWN_STATE = 	"NORMAL_ZOMBIE_DOWN_STATE";
    public static final String DYING_ZOMBIE_LEFT_STATE = 	"DYING_ZOMBIE_LEFT_STATE";
    public static final String DYING_ZOMBIE_RIGHT_STATE = 	"DYING_ZOMBIE_RIGHT_STATE";
    public static final String DEAD_ZOMBIE_LEFT_STATE = 	"DEAD_ZOMBIE_LEFT_STATE";
    public static final String DEAD_ZOMBIE_RIGHT_STATE = 	"DEAD_ZOMBIE_RIGHT_STATE";
    
    // ==GAMESQUARE==
    public static final String ACTIVE_GAME_SQUARE_STATE = 	"ACTIVE_GAME_SQUARE_STATE";
    public static final String DISABLED_GAME_SQUARE_STATE = "DISABLED_GAME_SQUARE_STATE";
    public static final String EMPTY_GAME_SQUARE_STATE = 	"EMPTY_GAME_SQUARE_STATE";
    
    // THESE ARE THE GUI SPRITE TYPES FOR THE MAIN MENU
    public static final String NEW_GAME_TYPE = 			"NEW_GAME";
    public static final String LEVEL_EDITOR_TYPE = 		"LEVEL_EDITOR_TYPE";
    public static final String PROFILE_EDITOR_TYPE = 	"PROFILE_EDITOR_TYPE";
    public static final String HIGH_SCORES_TYPE = 		"HIGH_SCORES_TYPE";
    public static final String CREDITS_TYPE = 			"CREDITS_TYPE";
    public static final String WON_GAME_TYPE = 			"WON_GAME_TYPE"; 
    public static final String LOSS_GAME_TYPE = 		"LOSS_GAME_TYPE";
    
    // AND HERE ARE ALL OUR GUI SETTINGS.
    public static final Font DEBUGGING_TEXT_FONT = new Font("Monospaced", Font.BOLD, 16);
    public static final Color DEBUGGING_TEXT_COLOR = Color.WHITE;
	
    // TODO: DYNAMIC ARRAY FOR STORING THE PANEL SPECIFIC STRING VALUES
//    public static final String settings[] = new String[];
	
	public MainMenu(String appTitle, int initFramesPerSecond) {
		super(appTitle, initFramesPerSecond);

	}

	@Override
	public void initData() {
		 // INIT OUR DATA MANAGER
        data = new MainMenuDataModel();
        data.setGameDimensions(GAME_WIDTH, GAME_HEIGHT);

        boundaryLeft = BOUNDARY_LEFT * GAME_WIDTH;
        boundaryRight = GAME_WIDTH - (GAME_WIDTH * BOUNDARY_RIGHT);
        boundaryTop = BOUNDARY_TOP * GAME_HEIGHT;
        boundaryBottom = GAME_HEIGHT - (GAME_HEIGHT * BOUNDARY_BOTTOM);

        // LOAD THE SPRITE TYPE INFO FROM THE FILE
        initSpriteTypes();

	}

	 /**
     * A helper method for initializing the data, this method loads
     * all the SpriteTypes from the SPRITE_TYPES_SETUP_FILE and
     * puts them in the data model.
     */
    private void initSpriteTypes() {
        // AND NOW LOAD IT WITH ALL THE DATA
        BufferedReader reader;
        try {
            FileReader fr = new FileReader(MAIN_MENU_SPRITE_TYPES_SETUP_FILE);
            reader = new BufferedReader(fr);

            // THE FIRST LINE IN THE FILE SHOULD LIST THE
            // NUMBER OF SPRITE TYPES TO BE DESCRIBED
            int numSpriteTypes = Integer.parseInt(reader.readLine());
            String line;
            String[] lineData;

            // NOW GET THEIR INFO ONE AT A TIME
            for (int i = 0; i < numSpriteTypes; i++) {
                // NOW FOR EACH SPRITE TYPE, EXTRACT
                // THE NAME AND THE NUMBER OF IMAGES
                line = reader.readLine();
                lineData = line.split(SETUP_DELIMITER);
                String spriteTypeID = lineData[0];
                int numImagesForSpriteType = Integer.parseInt(lineData[1]);
                SpriteType spriteType = new SpriteType(spriteTypeID);
                data.addSpriteType(spriteType);

                // NOW LOAD THE IMAGES FOR THIS SPRITE TYPE
                for (int j = 0; j < numImagesForSpriteType; j++) {
                    line = reader.readLine();
                    lineData = line.split(SETUP_DELIMITER);
                    String imageID = lineData[0];
                    String imageFileName = lineData[1];
                    Image imageToLoad = loadImageWithColorKey(imageFileName, COLOR_KEY_ORANGE);

                    // AND ADD THE STATE/IMAGE PAIRING
                    spriteType.addState(imageID, imageToLoad);
                }
            }
        } catch (Exception e) {
            JOptionPane.showMessageDialog(window, "Error Loading Game Data");
            System.exit(0);
        }
    }

    /**
     * For initializing all GUI controls, specifically all the
     * buttons and decor. Note that this method must construct
     * the canvas with its custom renderer.
     */
    @Override
    public void initGUIControls() {
        // CONSTRUCT THE PANEL WHERE WE'LL DRAW EVERYTHING
        canvas = new MainMenuPanel(this, (MainMenuDataModel) data);
        
        // WE'LL JUST SHARE THESE
        Image img;
        float x, y;
        SpriteType sT;
        Sprite s;

        // FIRST LOAD THE BACKGROUND, WHICH IS GUI DECOR
        sT = new SpriteType(BACKGROUND_TYPE);
        img = loadImage("./setup/images/MainMenu_Background.png");
        sT.addState(DEFAULT_STATE, img);
        x = 0;
        y = 0;
        s = new Sprite(sT, x, y, 0, 0, DEFAULT_STATE);
        guiDecor.put(BACKGROUND_TYPE, s);

        
        //TODO: AND THE WIN STATE 
        sT = new SpriteType(WON_GAME_TYPE);
        img = loadImageWithColorKey("./setup/images/WinDisplay.png", COLOR_KEY_WHITE);
        sT.addState(WON_GAME_STATE, img);
        x = (data.getGameWidth() - img.getWidth(null))/2;
        y = (data.getGameHeight() - img.getHeight(null))/2;
        s = new Sprite(sT, x, y, 0, 0, WON_GAME_STATE);
        guiDecor.put(WON_GAME_TYPE, s);
        
        //TODO: AND THE LOSS STATE 
        sT = new SpriteType(LOSS_GAME_TYPE);
        img = loadImageWithColorKey("./setup/images/LossDisplay.png", COLOR_KEY_ORANGE);
        sT.addState(LOSS_GAME_STATE, img);
        x = (data.getGameWidth() - img.getWidth(null))/2;
        y = (data.getGameHeight() - img.getHeight(null))/2;
        s = new Sprite(sT, x, y, 0, 0, LOSS_GAME_STATE);
        guiDecor.put(LOSS_GAME_TYPE, s);
  
        // AND NOW THE BUTTONS

        // FIRST THE NEW GAME BUTTON
        //TODO: Mouseover buttons arts and paths
        sT = new SpriteType(NEW_GAME_TYPE);
        img = loadImage("./setup/images/NewGame_Button.png");
        sT.addState(DEFAULT_STATE, img);
        img = loadImage("./setup/images/NewGame_Button.png");
        sT.addState(MOUSE_OVER_STATE, img);
        x = (data.getGameWidth() - img.getWidth(null)) / 2;
        y = 400;
        s = new Sprite(sT, x, y, 0, 0, DEFAULT_STATE);
        guiButtons.put(NEW_GAME_TYPE, s);
        
        // AND THE LEVEL EDITOR BUTTON
        //TODO: Mouseover buttons arts and paths
        sT = new SpriteType(LEVEL_EDITOR_TYPE);
        img = loadImageWithColorKey("./setup/images/LevelEditor2_Button.png", COLOR_KEY_ORANGE);
        sT.addState(DEFAULT_STATE, img);
        img = loadImage("./setup/images/LevelEditor2_Button.png");
        sT.addState(MOUSE_OVER_STATE, img);
        x = guiButtons.get(NEW_GAME_TYPE).getX();
        y = guiButtons.get(NEW_GAME_TYPE).getY() + 
        		guiButtons.get(NEW_GAME_TYPE).getSpriteType().getHeight() + 2;
        s = new Sprite(sT, x, y, 0, 0, DEFAULT_STATE);
        guiButtons.put(LEVEL_EDITOR_TYPE, s);

        // TODO: AND THE PROFILE EDITOR BUTTON
        
        // TODO: AND THE HIGH SCORES BUTTON
        
        // TODO: AND THE CREDITS BUTTON
        
        
    }
    

    /**
     * For initializing all the button handlers for the GUI.
     */
    @Override
    public void initGUIHandlers() {
        MainMenuButtonHandler mmbh = new MainMenuButtonHandler(this);
        guiButtons.get(NEW_GAME_TYPE).setActionListener(mmbh);
        guiButtons.get(LEVEL_EDITOR_TYPE).setActionListener(mmbh);           
    }


    /**
     * Called when a game is restarted from the beginning, it
     * resets all game data and GUI controls so that the game
     * may start anew.
     */
    @Override
    public void reset() {
        data.reset(this);
    }
    

    /***************************************************************************
     * Called each frame, this method updates the rendering state
     * of all relevant GUI controls, like displaying win and loss
     * states and whether certain buttons should be enabled or
     * disabled.
     **************************************************************************/
    @Override
    public void updateGUI() {
        // WE MAY NEED TO UPDATE GUI BUTTON STATES
        // DEPENDING ON THE GAME STATE, WHICH COULD
        // BE DONE HERE
        
        // GET THE DATA MODEL FROM THE CURRENT GAME
//        MainMenuDataModel zData = data;
        
        // UPDATE THE STATUS OF THE BUY ZOMBIE BUTTON IF THE PLAYER
        // HAS COLLECTED ENOUGH SUN
//        if(zData.getCollectedSun() >= 100){
//            guiButtons.get(BUY_ZOMBIE_TYPE).setState(ENABLED_STATE);
//        }else{
//            guiButtons.get(BUY_ZOMBIE_TYPE).setState(DISABLED_STATE);
//        }
        
        // UPDATE THE STATUS OF THE BUY TROPHY BUTTON IF THE PLAYER
        // HAS COLLECTED ENOUGH SUN
//        if( zData.getCollectedSun() >= 1000){
//            guiButtons.get(BUY_TROPHY_TYPE).setState(ENABLED_STATE);
//        }else{
//            guiButtons.get(BUY_TROPHY_TYPE).setState(DISABLED_STATE);
//        }
    }

    /**
     * The Zombiquarium game application starts here. All game data
     * and GUI initialization is done through the constructor, so
     * we will just construct our game and set it visible to start
     * it up.
     * 
     * @param args command line arguments, which will not be used
     */
    public static void main(String[] args) {
    	MainMenu mainMenu = new MainMenu(APP_TITLE, FRAME_RATE);
        mainMenu.startGame();
        //LevelEditorPP editor = new LevelEditorPP(APP_TITLE, FRAME_RATE);
        
      //MinesweeperPPPanel game = new MinesweeperPPPanel(APP_TITLE, FRAME_RATE);
      //game.startGame();
    }
	

}
