
#include "main_menu.h"
#include "particle_types.h"
#include "scene_graph.h"
#include "gui_image.h"
#include "test_scene.h"
#include "nicks_scene.h"
#include "charlies_scene.h"
#include "test_scene2.h"
#include "OBJmodel.h"
#include "platform.h"
#include "player.h"

MainMenu::MainMenu() :
m_showHowToPlay(false)
{
}

bool MainMenu::resetPlayer()
{
    if (m_player != NULL)
        m_player->destroy();
    
	// create the player in the scene
	m_player = new Player(this, 1.0f);
	Color glowColor = Color(0.8f, 0.9f, 0.8f, 1.0f);
	if ( !m_player->initialize(new LightTexShader(1.2f, 0.1f, 0, glowColor, false))) {
		GLOG("ERROR: Could not initialize player entity\n");
		return false;
	}
	m_player->setPosition(Vector3(0.0f, 2.0f, 10.0f));
	m_player->setYaw(-90);
	m_player->loadTexture("Textures/glass.tga");
	m_player->setGlowColor(glowColor);
	m_player->addToDynamicWorld(m_dynamicsWorld);
	registerForSimulationTicks(m_player);
	s_Cam()->attachTo(m_player);
    m_graph->attach(m_player);
	return true;
}


bool MainMenu::onInitialize()
{
    //position camera so that any 3D elements we attach to the graph are rendered how we want
    s_Cam()->setPosition(Vector3(0.0f, 15.0f, 40.0f));
    s_Cam()->setLookAt(Vector3(0.0f, 5.0f, 0.0f));
    s_Cam()->setCameraMode(CM_STILL);
    
    m_graph->pushNode();
	{
		//// create a platform
		Platform *platform = new Platform(this, Vector3(50.0f, 1.0f, 50.0f));
		if ( !platform->initialize(new LightShader(0.2f, 0.8f, 4, RAND_COLOR))) { GLOG("ERROR: Could not initialize center platform\n"); return false; }
		platform->setPosition(Vector3(0.0f, -0.5f, 0.0f));
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);
        
        //new OBJmodel here
        OBJmodel *game_title = new OBJmodel(this, "Objects/game_title.obj", 1.0f, false, false ,true);
        if ( !game_title->initialize(new LightShader(0.1f, 0.8, 8, RAND_LIGHT_COLOR))) { GLOG("ERROR: could not create game_title\n"); return false; }
        game_title->setPosition(Vector3(0.0f, game_title->getSize().y * 0.5f, 00.0f));
        game_title->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(game_title);
    }
    m_graph->popNode();
    
    // create particle system for the background
    /*ParticleSystem *randomSystem = new RandomParticleSystem(500);
     randomSystem->emitterPositionVar = Vector3(5.0f, 4.0f, 0.0f);
    randomSystem->startColor = Color(0.0f, 0.0f, 0.0f, 1.0f);
    randomSystem->endColor = Color(0.5f, 0.5f, 0.5f, 0.0f);
    randomSystem->endColorVar = Color(0.5f, 0.5f, 0.5f, 0.0f);
    randomSystem->speed = 0.1f;
    randomSystem->startSystem();
    m_graph->attach(randomSystem);*/
    
    float scale = s_Cam()->getScreenWidth() / 1024.0f;
    
    // create an image for nicks level
    nicks_level = new GUIImage();
    if ( !nicks_level->initialize(new TextureShader(), "Textures/nicks_level.tga"))
        return false;
    nicks_level->setPosition(Vector3(s_Cam()->getScreenWidth() * 0.25f, s_Cam()->getScreenHeight() - 100 * scale, 0.0f));
    nicks_level->setScale(scale);
    
    // create an image for charlies level
    charlies_level = new GUIImage();
    if ( !charlies_level->initialize(new TextureShader(), "Textures/charlies_level.tga"))
        return false;
    charlies_level->setPosition(Vector3(s_Cam()->getScreenWidth() * 0.75f, s_Cam()->getScreenHeight() - 100 * scale, 0.0f));
    charlies_level->setScale(scale);
    
    // create an image for how to play
    how_to_play_button = new GUIImage();
    if ( !how_to_play_button->initialize(new TextureShader(), "Textures/how_to_play_button.tga"))
        return false;
    how_to_play_button->setPosition(Vector3(s_Cam()->getScreenWidth() * 0.5f, s_Cam()->getScreenHeight() - 220 * scale, 0.0f));
    how_to_play_button->setScale(scale);
    
    // create an image for how to play
    how_to_play = new GUIImage();
    if ( !how_to_play->initialize(new TextureShader(), "Textures/how_to_play.tga"))
        return false;
    how_to_play->setPosition(Vector3(s_Cam()->getScreenWidth() * 0.5f, s_Cam()->getScreenHeight() * 0.5f, 0.0f));
    how_to_play->setScale(scale);
    
    return resetPlayer();
}

void MainMenu::onShutdown()
{
    delete nicks_level;
    delete charlies_level;
    delete how_to_play_button;
    delete how_to_play;
}

void MainMenu::onUpdate(float dt)
{
    if (s_Mouse()->isButtonPressed(0))
    {
        // toggle how to play off if it's on
        if (m_showHowToPlay) {
            m_showHowToPlay = false;
            return;
        }
        
		int x, y;
        s_Mouse()->getMousePos(x, y);
		y = s_Cam()->getScreenHeight() - y;
        if (nicks_level->isPointWithinImage(x, y)) {
            s_SceneManager()->loadScene(new NickScene());
        }
        else if (charlies_level->isPointWithinImage(x, y)) {
            s_SceneManager()->loadScene(new TestScene2());
        }
        else if (how_to_play_button->isPointWithinImage(x, y)) {
            m_showHowToPlay = true;
        }
    }
}

void MainMenu::onRender()
{
    if (!m_showHowToPlay) {
        nicks_level->render();
        charlies_level->render();
        how_to_play_button->render();
    }
    else {
        how_to_play->render();
    }
}



