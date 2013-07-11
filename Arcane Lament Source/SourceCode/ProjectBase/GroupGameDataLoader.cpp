#include "stdafx.h"
#include "SSSF_SourceCode\stdutil.h"

// GTKS INCLUDES
#include "SourceCode\ProjectBase\GroupGameDataLoader.h"
#include "SourceCode\ProjectBase\GroupGameData.h"
#include "SourceCode\ProjectBase\GroupGameTextGenerator.h"
#include "SourceCode\InGameCode\Renderers\InGameRenderer.h"
#include "SourceCode\InGameCode\InGameState.h"
#include "SourceCode\LuaModules\GameRendererModule.h"

#include "SSSF_SourceCode\dataLoader\ResourceManager.h"
#include "SSSF_SourceCode\dataLoader\SpriteResource.h"
#include "SSSF_SourceCode\dataLoader\GUIResource.h"
#include "SSSF_SourceCode\dataLoader\Resource.h"
#include "SSSF_SourceCode\dataLoader\ImageResource.h"
#include "SSSF_SourceCode\text\BufferedTextFileReader.h"

// GAME OBJECT INCLUDES
#include "SSSF_SourceCode\game\Game.h"
#include "SSSF_SourceCode\gsm\lualoader\LuaSystem.h"
#include "SSSF_SourceCode\graphics\GameGraphics.h"
#include "SSSF_SourceCode\gsm\state\GameState.h"
#include "SSSF_SourceCode\gsm\world\TiledLayer.h"
#include "SSSF_SourceCode\gsm\world\SparseLayer.h"
#include "SSSF_SourceCode\gui\OverlayImage.h"
#include "SSSF_SourceCode\gui\Cursor.h"
#include "SSSF_SourceCode\gui\GameGUI.h"
#include "SSSF_SourceCode\gui\ScreenGUI.h"
#include "SSSF_SourceCode\input\GameInput.h"
#include "SSSF_SourceCode\os\GameOS.h"
#include "SSSF_SourceCode\text\GameText.h"
#include "SSSF_SourceCode\gsm\ai\BotHandler.h"
#include "SSSF_SourceCode\graphics\OverlayRenderer.h"
#include <SSSF_SourceCode\gsm\physics\DynamicBody.h>
#include <SSSF_SourceCode\thread\GameThread.h>
#include <SourceCode\ProjectBase\GameThreads.h>

// WINDOWS PLATFORM INCLUDES
#include "SSSF_SourceCode\PlatformPlugins\WindowsPlugin\WindowsOS.h"
#include "SSSF_SourceCode\PlatformPlugins\WindowsPlugin\WindowsInput.h"
#include "SSSF_SourceCode\PlatformPlugins\WindowsPlugin\WindowsTimer.h"

// DIRECTX INCLUDES
#include "SSSF_SourceCode\PlatformPlugins\DirectXPlugin\DirectXGraphics.h"
#include "SSSF_SourceCode\PlatformPlugins\DirectXPlugin\DirectXTextureManager.h"

/*
	loadGame - This method loads the setup game data into the game and
	constructs all the needed objects for the game to work.
*/

void GroupGameDataLoader::loadGame(Game *game, wstring gameInitFile)
{
	// AND LET'S READ IN THE GAME SETUP INFO
	// FIRST LOAD ALL THE PROPERTIES
	map<wstring,wstring> *properties = new map<wstring,wstring>();
	loadGameProperties(game, properties, gameInitFile);
	
	//this->setGameDataPathName(*wstr);
	// WE NEED THE TITLE AND USE_FULLSCREEN_MODE TO INITIALIZE OUR WINDOW
	wstring titleProp = (*properties)[GG_TITLE];
	wstring useFullscreenProp = (*properties)[GG_USE_FULLSCREEN_MODE];
	bool useFullscreen = false;
	if (useFullscreenProp.compare(L"true") == 0)
		useFullscreen = true;

	// GET THE SCREEN WIDTH AND HEIGHT
	int screenWidth, screenHeight;
	wstring screenWidthProp = (*properties)[GG_SCREEN_WIDTH];
	wstring screenHeightProp = (*properties)[GG_SCREEN_HEIGHT];
	wstringstream(screenWidthProp) >> screenWidth;
	wstringstream(screenHeightProp) >> screenHeight;

	// MAKE A CUSTOM GameOS OBJECT (WindowsOS) FOR SOME WINDOWS
	// PLATFORM STUFF, INCLUDING A Window OF COURSE
	WindowsOS *dummyOS = new WindowsOS(	hInstance, 
										nCmdShow,
										useFullscreen,
										titleProp,
										screenWidth, screenHeight,
										game);
	
	int textFontSize;
	wstring textFontSizeProp = (*properties)[GG_TEXT_FONT_SIZE];
	wstringstream(textFontSizeProp) >> textFontSize;

	// RENDERING WILL BE DONE USING DirectX
	DirectXGraphics *dummyGraphics = new DirectXGraphics(game);
	dummyGraphics->init(screenWidth, screenHeight);
	dummyGraphics->initGraphics(dummyOS->getWindowHandle(), useFullscreen);
	dummyGraphics->initTextFont(textFontSize);

	
	
	// AND NOW LOAD THE COLORS THE GRAPHICS WILL USE
	// AS A COLOR KEY AND FOR RENDERING TEXT
	initColors(dummyGraphics, properties);

	// WE'LL USE WINDOWS PLATFORM METHODS FOR GETTING INPUT
	WindowsInput *dummyInput = new WindowsInput();

	// AND WINDOWS FOR THE TIMER AS WELL
	WindowsTimer *dummyTimer = new WindowsTimer();

	// NOW INITIALIZE THE Game WITH ALL THE
	// PLATFORM AND GAME SPECIFIC DATA WE JUST CREATED
	game->initPlatformPlugins(	(GameGraphics*)dummyGraphics,
								(GameInput*)dummyInput,
								(GameOS*)dummyOS,
								(GameTimer*)dummyTimer,
								new GameThreads());
	// Load in Resouces into Resource Manager
	// LOAD OUR CUSTOM TEXT GENERATOR, WHICH DRAWS
	// TEXT ON THE SCREEN EACH FRAME
	GroupGameTextGenerator *dummyTextGenerator = new GroupGameTextGenerator();
	dummyTextGenerator->initText(game);
	GameText *text = game->getText();
	text->setTextGenerator((TextGenerator*)dummyTextGenerator);

	// INIT THE VIEWPORT TOO
	initViewport(game->getGUI(), properties);	
	
	//Add Lua Modules
	GameRendererModule *module=new GameRendererModule;
	module->renderer=new InGameRenderer();
	game->getLuaSys()->addModule("InGameRendererModule",module);
	
	// WE DON'T NEED THE PROPERTIES MAP ANYMORE, THE GAME IS ALL LOADED
	delete properties;
}

/*
	initColors - this helper method loads the color key, used for loading
	images, and the font color, used for rendering text.
*/
void GroupGameDataLoader::initColors(	GameGraphics *graphics,
									map<wstring,wstring> *properties)
{
	int fontRed, fontGreen, fontBlue;
	wstring fontRedProp = (*properties)[GG_FONT_COLOR_RED];
	wstring fontGreenProp = (*properties)[GG_FONT_COLOR_GREEN];
	wstring fontBlueProp = (*properties)[GG_FONT_COLOR_BLUE];
	wstringstream(fontRedProp) >> fontRed;
	wstringstream(fontGreenProp) >> fontGreen;
	wstringstream(fontBlueProp) >> fontBlue;

	// COLOR USED FOR RENDERING TEXT
	graphics->setFontColor(fontRed, fontGreen, fontBlue);
}

/*
	loadGUI - This method loads all the GUI assets described in the guiInitFile
	argument. Note that we are loading all GUI art for all GUIs when the application
	first starts. We'll learn a better technique later in the semester.
*/
void GroupGameDataLoader::loadGUI(Game *game, wstring guiInitFile)
{
	LuaSystem *ls=game->getLuaSys();
	lua_State* state=ls->getNewLuaState();
	game->getGUI()->registerLua(state);
	ScreenGUI::registerLua(state);
	//Execute gui file~!!!!
	string s="";
	if(luaL_dofile(state,"data/lua/guiInitFile.lua")==1) s="Couldn't run Lua program: "+(string)lua_tostring(state, -1);
	cout<<s;
	lua_close(state);
}



struct TileData{
	float x, y;
		int imgID;
	};
/*
	loadLevel - This method should load the data the level described by the
	levelInitFile argument in to the Game's game state manager.
*/
void GroupGameDataLoader::loadWorld(Game *game, wstring levelInitFile)	
{
	LuaSystem *ls=game->getLuaSys();
	lua_State* state=ls->getNewLuaState();
	//Execute gui file~!!!!
	string s="";
	if(luaL_dofile(state,convertWStr(levelInitFile).c_str())==1) 
		s="Couldn't run Lua program: "+(string)lua_tostring(state, -1);
	cout<<s;
	string levelFile=luabind::call_function<string>(state,"getLevelFile");
	Viewport *vp = game->getGUI()->getViewport();
	GameStateManager *gsm = game->getGSM();
	GameGraphics *graphics = game->getGraphics();
	TextureManager *worldTextureManager = graphics->getWorldTextureManager();
	SpriteManager *spriteManager = gsm->getSpriteManager();
	//Unload old world
	gsm->unloadCurrentLevel(game);
	gsm->getPhysics()->reset();
	
	
	//Old Level Init File
	BufferedTextFileReader reader;
	reader.initFile(convertwstrX(levelFile));
	wstring dataIn=L"";
	while(reader.hasMoreLines())
		dataIn=dataIn+reader.getNextLine();
	//look for player info
	int index=dataIn.find(L",");
	//Look for number of Tile Info
	int numTiles= _wtoi(dataIn.substr(0,index).c_str());
	dataIn=dataIn.substr(index+1);
	vector<wstring> tiles;
	vector<int> collTiles;
	//get tile ids
	while(numTiles--!=0){
		index=dataIn.find(L",");
		tiles.push_back(dataIn.substr(0,index));
		dataIn=dataIn.substr(index+1);
		index=dataIn.find(L",");
		collTiles.push_back(_wtoi(	dataIn.substr(0,index).c_str()));
		dataIn=dataIn.substr(index+1);
	}
	index=dataIn.find(L",");
	int nLayers= _wtoi(dataIn.substr(0,index).c_str());
	dataIn=dataIn.substr(index+1);
	//get w and h
	index=dataIn.find(L",");
	int w= _wtoi(dataIn.substr(0,index).c_str());
	dataIn=dataIn.substr(index+1);
	index=dataIn.find(L",");
	int h= _wtoi(dataIn.substr(0,index).c_str());
	dataIn=dataIn.substr(index+1);
	World *world = gsm->getWorld();
	world->setWorldWidth(h * TILE_WIDTH);
	world->setWorldHeight(w * TILE_HEIGHT);
	int *tileIDs = new int[tiles.size()];
	for(unsigned int i=0;i<tiles.size();i++)
		tileIDs[i]=worldTextureManager->loadTexture(tiles[i]);
	vector <TileData> imgs;
	while(nLayers-->0){
		index=dataIn.find(L",");
		int layerType= _wtoi(dataIn.substr(0,index).c_str());
		dataIn=dataIn.substr(index+1);

		if(layerType==0){
			int **grid = new int*[w];
			for(int i=0;i<w;i++)grid[i]=new int[h];
			for(int i=0;i<w;i++)
				for(int j=0;j<h;j++){
					index=dataIn.find(L",");
					grid[i][j]= _wtoi(dataIn.substr(0,index).c_str());
					dataIn=dataIn.substr(index+1);
				}
			// Create Tiled layer
			TiledLayer *tiledLayer = new TiledLayer(	w,	h, 
														TILE_WIDTH,		TILE_HEIGHT, 
														0, true, 
														w * TILE_WIDTH,
														h * TILE_HEIGHT);
			
			for (int j=0;j<h;j++)
				for(int i = 0; i < (w); i++)
				{
					if(grid[i][j]!=-1){
						Tile *tileToAdd = new Tile();
						if( 0!=collTiles[grid[i][j]]){
							tileToAdd->collidable = 0==collTiles[0];
							tileToAdd->textureID = tileIDs[0];
							tiledLayer->addTile(tileToAdd);
							TileData d;
							d.x=i*TILE_WIDTH;
							d.y=j*TILE_HEIGHT;
							d.imgID=collTiles[grid[i][j]];
							imgs.push_back(d);
						}
						else{
							tileToAdd->collidable = 0==collTiles[grid[i][j]];
							tileToAdd->textureID = tileIDs[grid[i][j]];
							tiledLayer->addTile(tileToAdd);
						}
					}
					else
					{
						tiledLayer->addTile(NULL);
					}
				}
	
			for(int i=0;i<w;i++)delete [] grid[i];
			delete [] grid;
			world->addLayer(tiledLayer);
		}
		else if(layerType==1){
			SparseLayer *newLayer = new SparseLayer();
			index=dataIn.find(L",");
			newLayer->fx=(float)atof(convertWStr(dataIn.substr(0,index).c_str()).c_str());
			dataIn=dataIn.substr(index+1);
			index=dataIn.find(L",");
			newLayer->fy= (float)atof(convertWStr(dataIn.substr(0,index).c_str()).c_str());
			dataIn=dataIn.substr(index+1);
			index=dataIn.find(L",");
			int numObjs= _wtoi(dataIn.substr(0,index).c_str());
			dataIn=dataIn.substr(index+1);
			newLayer->setCollidableTiles(false);
			for(int i=0;i<imgs.size();i++){
				OverlayImage *img = new OverlayImage();
				img->imageID=imgs[i].imgID;
				img->x=(int)(imgs[i].x);
				img->y=(int)(imgs[i].y);
				img->width =worldTextureManager->getTextureX(img->imageID);
				img->height=worldTextureManager->getTextureY(img->imageID);
				img->alpha=255;
				img->parent=newLayer;
				addImageToPhysics(gsm->getPhysics(),img,0);
				newLayer->addTile(img);
			}
			imgs.clear();
			while(numObjs-->0){
				OverlayImage *img = new OverlayImage();
				index=dataIn.find(L",");
				img->imageID= worldTextureManager->loadTexture(dataIn.substr(0,index));
				dataIn=dataIn.substr(index+1);
				index=dataIn.find(L",");
				float x= (float)atof(convertWStr(dataIn.substr(0,index).c_str()).c_str());
				dataIn=dataIn.substr(index+1);
				index=dataIn.find(L",");
				float y= (float)atof(convertWStr(dataIn.substr(0,index).c_str()).c_str());
				dataIn=dataIn.substr(index+1);
				index=dataIn.find(L",");
				int type= _wtoi(dataIn.substr(0,index).c_str());
				dataIn=dataIn.substr(index+1);
				img->x=(int)(x*TILE_WIDTH);
				img->y=(int)(y*TILE_HEIGHT);
				img->width =worldTextureManager->getTextureX(img->imageID);
				img->height=worldTextureManager->getTextureY(img->imageID);
				img->alpha=255;
				img->parent=newLayer;
				addImageToPhysics(gsm->getPhysics(),img,type);
				newLayer->addTile(img);
				
			}
			world->addLayer(newLayer);
		}
	}
	delete[] tileIDs;


	
	gsm->setManager((new InGameState(game,state)));
	((InGameState*)gsm->getManager())->initState();
	gsm->getPhysics()->setupBounds(0,0,(float)(w*TILE_WIDTH)*BOX2D_Scale_Factor,(float)(h*TILE_HEIGHT)*BOX2D_Scale_Factor);
	
	//Load in Bots
	//game->getGraphics()->getResourceManager()->loadInBots(game);
	//Clear Particle System
	//game->getGSM()->getFXSys()->clearSystem();
	//Load Particle System
	//loadFX(game);
	
	lua_close(state);
}


void GroupGameDataLoader::addImageToPhysics(Physics* p, OverlayImage* image,int type){
	b2BodyDef obj;
	obj.position.x=(image->x+image->width/2.0f)*BOX2D_Scale_Factor;
	obj.position.y=(image->y+image->height/2.0f)*BOX2D_Scale_Factor;
	obj.fixedRotation=true;
	image->hook->data=image;
	if(type==0)
		image->hook->type=0;
	if(type==1)
		image->hook->type=1;
	obj.userData=image->hook;
	b2Body *body=p->createBody(&obj);
	b2PolygonShape box;
	box.SetAsBox(image->width/2.0f*BOX2D_Scale_Factor,image->height/2.0f*BOX2D_Scale_Factor);
	body->CreateFixture(&box,0.0f);
}
/*
	initViewport - initializes the game's viewport.
*/
void GroupGameDataLoader::initViewport(GameGUI *gui, map<wstring,wstring> *properties)
{
	// AND NOW SPECIFY THE VIEWPORT SIZE AND LOCATION. NOTE THAT IN THIS EXAMPLE,
	// WE ARE PUTTING A TOOLBAR WITH A BUTTON ACCROSS THE NORTH OF THE APPLICATION.
	// THAT TOOLBAR HAS A HEIGHT OF 64 PIXELS, SO WE'LL MAKE THAT THE OFFSET FOR
	// THE VIEWPORT IN THE Y AXIS
	Viewport *viewport = gui->getViewport();

	int viewportOffsetX, viewportOffsetY,vpbotoffx,vpbotoffy, screenWidth, screenHeight;
	wstring viewportOffsetXProp = (*properties)[GG_VIEWPORT_OFFSET_X];
	wstring viewportOffsetYProp = (*properties)[GG_VIEWPORT_OFFSET_Y];
	wstring viewportBotOffsetXProp = (*properties)[GG_VIEWPORT_BOTTOM_OFFSET_X];
	wstring viewportBotOffsetYProp = (*properties)[GG_VIEWPORT_BOTTOM_OFFSET_Y];
	wstring screenWidthProp = (*properties)[GG_SCREEN_WIDTH];
	wstring screenHeightProp = (*properties)[GG_SCREEN_HEIGHT];
	wstringstream(viewportOffsetXProp) >> viewportOffsetX;
	wstringstream(viewportOffsetYProp) >> viewportOffsetY;
	wstringstream(screenWidthProp) >> screenWidth;
	wstringstream(screenHeightProp) >> screenHeight;
	wstringstream(viewportBotOffsetXProp) >> vpbotoffx; 
	wstringstream(viewportBotOffsetYProp) >> vpbotoffy; 
	int viewportWidth = screenWidth - viewportOffsetX-vpbotoffx;
	int viewportHeight = screenHeight - viewportOffsetY-vpbotoffy;
	viewport->setViewportWidth(viewportWidth);
	viewport->setViewportHeight(viewportHeight);
	viewport->setViewportOffsetX(viewportOffsetX);
	viewport->setViewportOffsetY(viewportOffsetY);
}
