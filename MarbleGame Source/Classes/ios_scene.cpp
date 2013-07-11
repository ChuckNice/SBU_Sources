#include "ios_scene.h"
#include "scene_graph.h"
#include "cube.h"
#include "cube_groups.h"
#include "platform.h"
#include "player.h"
#include "trampoline.h"
#include "cannon.h"
#include "button_types.h"
#include "particle_types.h"
#include "OBJmodel.h"
#include "fracture.h"

iOSScene::iOSScene() :
Scene()
{
}

bool iOSScene::resetPlayer()
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
	m_player->setPosition(Vector3(10.0f, 2.0f, 0.0f));
	m_player->setYaw(180.0f);
	m_player->loadTexture("Textures/glass.tga");
	m_player->setGlowColor(glowColor);
	m_player->addToDynamicWorld(m_dynamicsWorld);
	registerForSimulationTicks(m_player);
	s_Cam()->attachTo(m_player);
    m_graph->attach(m_player);
	return true;
}

bool iOSScene::onInitialize()
{
	//-----------------------------------------------------------------
	// create the center platform, which consists of 2 cube pyramids,
	// and a cannon (with button) that is aimed at another platform
	// with a cube pyramid
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
		//// create a platform
		Platform *platform = new Platform(this, Vector3(40.0f, 1.0f, 40.0f));
		if ( !platform->initialize(new LightShader(0.1f, 1.0f, 4, RAND_COLOR))) { GLOG("ERROR: Could not initialize center platform\n"); return false; }
		platform->setPosition(Vector3(0.0f, -0.5f, 0.0f));
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);

		//// add cube pyramid
		m_graph->pushNode();
		Vector3 cubeSize = Vector3(1.5f, 1.5f, 0.5f);
		Vector3 startPoint = Vector3(-6.0f, cubeSize.y / 2.0f, -11.0f);
		CubeGroups::createCubePyramid(this, 5, startPoint, cubeSize, 0.2f);
		m_graph->popNode();

		// add cube pyramid
		m_graph->pushNode();
		cubeSize = Vector3(1.5f, 1.5f, 0.5f);
		startPoint = Vector3(-6.0f, cubeSize.y / 2.0f, 11.0f);
		CubeGroups::createCubePyramid(this, 5, startPoint, cubeSize, 0.2f);
		m_graph->popNode();

		//// add a cannon aiming at the other platform
		Cannon *cannon = new Cannon(this, Vector3(1.5f, 1.5f, 5.0f), 40.0f);
		if (!cannon->initialize(new LightShader(0.2f, 1.0f, 8, Color(0.1f, 0.8f, 0.2f)))) { GLOG("ERROR: could not initialize first cannon\n"); return false; }
		cannon->setPosition(Vector3(-28.0f, 0.0f, 4.0f));
		cannon->setOrientation(90.0f, 30.0f, 0.0f);
		cannon->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(cannon);

		//// add a button aiming to shoot the above cannon
		CannonButton *cannon_button = new CannonButton(this, Vector3(4.0f, 0.5f, 4.0f), 1.0f, cannon);
		if (!cannon_button->initialize(new LightShader(0.3f, 1.0f, 8, RAND_COLOR))) { GLOG("ERROR: could not create first cannon button\n"); return false; }
		cannon_button->setPosition(Vector3(-17.0f, cannon_button->getSize().y * 0.5f, 1.0f));
		cannon_button->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(cannon_button);

		//new OBJmodel here
		OBJmodel *hydralisk = new OBJmodel(this, "Objects/hydralisk.obj", 50.0f, false, false ,true);
		if ( !hydralisk->initialize(new LightTexShader(0.2f, 1.0, 8, Color(1.0f, 1.0f, 1.0f)))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
		hydralisk->setPosition(Vector3(0.0f, 0.0f, 80.0f));
		hydralisk->setYaw(180.0f);
		hydralisk->loadTexture("Textures/hydralisk.tga");
		hydralisk->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(hydralisk);
	}
	m_graph->popNode();


	//-----------------------------------------------------------------
	// create the platform to the left which has the cannon from the center
	// platform aiming at it. has a cube pyramid on it
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
		// create a platform
		Platform *platform = new Platform(this, Vector3(10.0f, 2.0f, 20.0f));
		if ( !platform->initialize(new LightShader(0.1f, 1.0f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize formard platform\n"); return false; }
		platform->setPosition(Vector3(-70.0f, -1.0f, 4.0f));
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);

		// add cube pyramid
		m_graph->pushNode();
		Vector3 cubeSize = Vector3(1.5f, 1.5f, 0.5f);
		Vector3 startPoint = Vector3(-71.0f, cubeSize.y / 2.0f, 9.0f);
		CubeGroups::createCubePyramid(this, 6, startPoint, cubeSize, 0.2f, -90.0f);
		m_graph->popNode();
	}
	m_graph->popNode();


	//-----------------------------------------------------------------
	// create a line of angled trampolines leading to a platform 
	//-----------------------------------------------------------------
	/*m_graph->pushNode();
	{
		// first trampoline
		Trampoline *tramp = new Trampoline(this, 10.0f, 15.0f);
		if ( !tramp->initialize(new TrampolineShader()) ) { GLOG("ERROR: Could not initialize first entity\n"); return false; }
		tramp->setPosition(Vector3(-6.0, 0.0f, -30.0f));
		tramp->setRoll(30.0f);
		tramp->addToDynamicWorld(m_dynamicsWorld);
		registerForSimulationTicks(tramp);
		m_graph->attach(tramp);

		// second trampoline
		tramp = new Trampoline(this, 10.0f, 15.0f);
		if ( !tramp->initialize(new TrampolineShader()) ) { GLOG("ERROR: Could not initialize second entity\n"); return false; }
		tramp->setPosition(Vector3(6.0f, 0.0f, -45.0f));
		tramp->setRoll(-30.0f);
		tramp->addToDynamicWorld(m_dynamicsWorld);
		registerForSimulationTicks(tramp);
		m_graph->attach(tramp);

		// third trampoline
		tramp = new Trampoline(this, 10.0f, 15.0f);
		if ( !tramp->initialize(new TrampolineShader()) ) { GLOG("ERROR: Could not initialize third entity\n"); return false; }
		tramp->setPosition(Vector3(-6.0f, 0.0f, -60.0f));
		tramp->setRoll(30.0f);
		tramp->addToDynamicWorld(m_dynamicsWorld);
		registerForSimulationTicks(tramp);
		m_graph->attach(tramp);

		// fourth trampoline
		tramp = new Trampoline(this, 10.0f, 15.0f);
		if ( !tramp->initialize(new TrampolineShader()) ) { GLOG("ERROR: Could not initialize fourth entity\n"); return false; }
		tramp->setPosition(Vector3(6.0f, 0.0f, -75.0f));
		tramp->setRoll(-30.0f);
		tramp->addToDynamicWorld(m_dynamicsWorld);
		registerForSimulationTicks(tramp);
		m_graph->attach(tramp);
	}
	m_graph->popNode();


	//-----------------------------------------------------------------
	// create a platform after the line of trampolines that has some 
	// cube pyramids on it
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
		// create a platform
		Platform *platform = new Platform(this, Vector3(50.0f, 1.0f, 50.0f));
		if ( !platform->initialize(new LightShader(0.1f, 1.0f, 4, RAND_COLOR))) { GLOG("ERROR: Could not initialize right platform\n"); return false; }
		platform->setPosition(Vector3(0.0f, -0.5f, -115.0f));
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);

		// add cube pyramid
		m_graph->pushNode();
		Vector3 cubeSize = Vector3(1.0f, 1.0f, 1.0f);
		Vector3 startPoint = Vector3(-10.0f, cubeSize.y / 2.0f, -110.0f);
		CubeGroups::createCubePyramid(this, 5, startPoint, cubeSize, 0.1f, -90.0f);
		m_graph->popNode();

		// add cube pyramid
		m_graph->pushNode();
		cubeSize = Vector3(1.0f, 2.0f, 0.5f);
		startPoint = Vector3(10.0f, cubeSize.y / 2.0f, -110.0f);
		CubeGroups::createCubePyramid(this, 6, startPoint, cubeSize, 0.1f, -90.0f);
		m_graph->popNode();

		// add cube pyramid
		m_graph->pushNode();
		cubeSize = Vector3(1.0f, 1.0f, 1.0f);
		startPoint = Vector3(-7.0f, cubeSize.y / 2.0f, -130.0f);
		CubeGroups::createCubePyramid(this, 8, startPoint, cubeSize, 0.3f, 0.0f);
		m_graph->popNode();
	}
	m_graph->popNode();*/
    
    
    // create a particle system
    ParticleSystem *m_particleSystem = new FireParticleSystem(300);
    //ParticleSystem *m_particleSystem = new RandomParticleSystem(300);
    m_particleSystem->moveToPoint(0.0f, 0.4f, 0.0f);
    m_particleSystem->startSystem();
    m_graph->attach(m_particleSystem);
    

	// create the player and return result
	return resetPlayer();
}


void iOSScene::onUpdate(float dt)
{
}

void iOSScene::onRender()
{
}

void iOSScene::onShutdown()
{
}