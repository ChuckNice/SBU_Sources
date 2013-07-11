#include "test_scene2.h"
#include "scene_graph.h"
#include "cube.h"
#include "cube_groups.h"
#include "platform.h"
#include "player.h"
#include "trampoline.h"
#include "cannon.h"
#include "button_types.h"

#include "OBJmodel.h"
#include "conveyor.h"
#include "particle_types.h"
#include "waypoint_types.h"
#include "fracture.h"
#include "collectible.h"

TestScene2::TestScene2() :
Scene()
{
}

bool TestScene2::resetPlayer()
{
    if (m_player != NULL) {
        unregisterForSimulationTicks(m_player);
        m_player->destroy();
    }
    
	// create the player in the scene
	m_player = new Player(this, 1.0f);
	Color glowColor = Color(0.8f, 0.9f, 0.9f, 1.0f);
	if ( !m_player->initialize(new LightTexShader(1.2f, 0.1f, 0, glowColor, true))) {
		GLOG("ERROR: Could not initialize player entity\n");
		return false;
	}
	m_player->setPosition(m_WPstart->getPosition() + Vector3(0.0f, 0.5f, 0.0f));
	m_player->setYaw(0.0f);
	m_player->loadTexture("Textures/glass.tga");
	m_player->setGlowColor(glowColor);
	m_player->addToDynamicWorld(m_dynamicsWorld);
	registerForSimulationTicks(m_player);
	s_Cam()->attachTo(m_player);
    m_graph->attach(m_player);
	return true;
}

static FractureObject *sphere_fracture = NULL;

bool TestScene2::onInitialize()
{
	
	//-----------------------------------------------------------------
	// create a conveyor to the right of center platform
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
		//// create a conveyor
		Conveyor *conveyor1 = new Conveyor(this, Vector3(10.0f, 1.0f, 40.0f), 1.0f );
		if ( !conveyor1->initialize(new LightTexScrollShader(0.1f, 1.0f, 4, Color(1.0f, 1.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f), conveyor1->getSpeed(), 3.0f))) { GLOG("ERROR: Could not initialize conveyor1\n"); return false; }
		conveyor1->loadTexture("Textures/conveyor.tga");
		conveyor1->setPosition(Vector3(50.0f, 3.0f, 0.0f));
		conveyor1->setYaw(270.0f);
		//conveyor1->setPitch(0.0f);
		//conveyor1->setPitch(30.0);
		conveyor1->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(conveyor1);

		////add a dynamic platform made of cubes that disintegrates when touched
		
		OBJmodel *slide = new OBJmodel(this, "Objects/slide2.obj", false, false, false);
		if ( !slide->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
		slide->setPosition(Vector3(35.0f, -25.0f, -10.0f));
		slide->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(slide);

		////downtube test
		OBJmodel *downtube = new OBJmodel(this, "Objects/downtube.obj", 2.0, false, false, false);
		if (!downtube->initialize(new LightShader(0.1f, 1.0f, 4, RAND_COLOR))) { GLOG("ERROR: Could not initialize downtube"); return false; }
		downtube->setPosition(Vector3(65.0f, -25.0f, 10.0f));
		downtube->setYaw(180.0f);
		downtube->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(downtube);

		////second conveyor rotated 90 to first
		Conveyor *conveyor2 = new Conveyor(this, Vector3(10.0f, 1.0f, 40.0f), 1.0f );
		if ( !conveyor2->initialize(new LightTexScrollShader(0.1f, 1.0f, 4, Color(1.0f, 1.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f), conveyor2->getSpeed(), 3.0f))) { GLOG("ERROR: Could not initialize conveyor2\n"); return false; }
		conveyor2->loadTexture("Textures/conveyor.tga");
		conveyor2->setPosition(Vector3(50.0f, 3.0f, 25.0f));
		conveyor2->setYaw(0.0f);
		conveyor2->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(conveyor2);
	}
	m_graph->popNode();


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
		//platform->setPitch(30.0f);
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);

		//// String of collectibles
		Collectible *coll_1 = new Collectible(this, "Objects/boxxy.obj", 100, 1.0f );
		if ( !coll_1->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create Collectible\n"); return false; }
		coll_1->setPosition(Vector3(-5.0f, 3.0f, 0.0f));
		coll_1->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(coll_1);
		Collectible *coll_2 = new Collectible(this, "Objects/boxxy.obj", 10, 1.0f);
		if ( !coll_2->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create Collectible\n"); return false; }
		coll_2->setPosition(Vector3(-7.0f, 3.0f, 0.0f));
		coll_2->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(coll_2);
		Collectible *coll_3 = new Collectible(this, "Objects/boxxy.obj", 30, 1.0f);
		if ( !coll_3->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create Collectible\n"); return false; }
		coll_3->setPosition(Vector3(-9.0f, 3.0f, 0.0f));
		coll_3->addToDynamicWorld(m_dynamicsWorld);
		//registerForSimulationTicks(coll_3);
		m_graph->attach(coll_3);

		//start position Waypoint this is where the player starts/resets
		m_WPstart  = new Start(this, "Objects/goal.obj", 0.125f);
		if ( !m_WPstart->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
		m_WPstart->setPosition(Vector3(10.0f, 0.5f, 0.0f));
		m_WPstart->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(m_WPstart);

		Checkpoint *cp = new Checkpoint(this, "Objects/goal.obj", 0.125f);
		if ( !cp->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
		cp->setPosition(Vector3(15.0f, 0.5f, 0.0f));
		m_allCheckpoints.push_back(cp);
		cp->addToDynamicWorld(m_dynamicsWorld);
		ParticleSystem *ps = new WaypointParticleSystem(300);
		cp->attachParticleSystem(ps);
		m_graph->attach(ps);
		m_graph->attach(cp);

		// create a particle system
		//ParticleSystem *m_particleSystem = new CharlieParticleSystem(300);
		ParticleSystem *m_particleSystem = new RandomParticleSystem(300);
		//ParticleSystem *m_particleSystem = new FireParticleSystem(300);
		m_particleSystem->moveToPoint(-10.0f, 0.4f, 0.0f);
		m_particleSystem->startSystem();
		m_graph->attach(m_particleSystem);

		//// add cube pyramid
		m_graph->pushNode();
		Vector3 cubeSize = Vector3(1.5f, 1.5f, 0.5f);
		Vector3 startPoint = Vector3(-6.0f, cubeSize.y / 2.0f, -11.0f);
		CubeGroups::createCubePyramid(this, 6, startPoint, cubeSize, 0.2f);
		m_graph->popNode();

		// add cube pyramid
		m_graph->pushNode();
		cubeSize = Vector3(2.2f, 1.0f, 1.2f);
		startPoint = Vector3(-6.0f, cubeSize.y / 2.0f, 11.0f);
		CubeGroups::createCubePyramid(this, 6, startPoint, cubeSize, 0.05f);
		m_graph->popNode();

		//// add a spinning cannon aiming at the other platform
		Cannon *cannon = new Cannon(this, Vector3(1.5f, 1.5f, 5.0f), 40.0f, -1.0f, 1.0f);
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
		OBJmodel *hydralisk = new OBJmodel(this, "Objects/hydralisk.obj", 50.0f, false, false, true);
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
		CubeGroups::createCubePyramid(this, 7, startPoint, cubeSize, 0.2f, -90.0f);
		m_graph->popNode();

		//// create a fracture sphere that explodes
		//Sphere *sphere = new Sphere(this, 5.0f);
		//if ( !sphere->initialize(new LightTexShader(0.5f, 1.0f, 6, Color(1.0f, 1.0f, 1.0f, 1.0f)))) { GLOG("ERROR: could not initialize sphere.\n"); return false; }
		//sphere->setPosition(Vector3(-40.0f, 3.0f, 5.0f));
		//sphere->setOrientation(0.0f, -90.0f, 0.0f);
		//sphere->loadTexture("Textures/earth.tga");
		//sphere_fracture = new FractureObject(sphere, "Textures/med_fracture.tga", 10.0f);
		//if ( !sphere_fracture->addToScene(this) ) { GLOG("ERROR: could not add fracture to scene.\n"); return false; }
	
	}
	m_graph->popNode();


	//-----------------------------------------------------------------
	// create a line of angled trampolines leading to a platform 
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
		// first trampoline
		Trampoline *tramp = new Trampoline(this, 10.0f, 15.0f);
		if ( !tramp->initialize(new TrampolineShader()) ) { GLOG("ERROR: Could not initialize first entity\n"); return false; }
		tramp->loadTexture("Textures/trampoline.tga");
		tramp->setPosition(Vector3(-6.0, 0.0f, -30.0f));
		tramp->setRoll(30.0f);
		tramp->addToDynamicWorld(m_dynamicsWorld);
		registerForSimulationTicks(tramp);
		m_graph->attach(tramp);

		// second trampoline
		tramp = new Trampoline(this, 10.0f, 15.0f);
		if ( !tramp->initialize(new TrampolineShader()) ) { GLOG("ERROR: Could not initialize second entity\n"); return false; }
		tramp->loadTexture("Textures/trampoline.tga");
		tramp->setPosition(Vector3(6.0f, 0.0f, -45.0f));
		tramp->setRoll(-30.0f);
		tramp->addToDynamicWorld(m_dynamicsWorld);
		registerForSimulationTicks(tramp);
		m_graph->attach(tramp);

		// third trampoline
		tramp = new Trampoline(this, 10.0f, 15.0f);
		if ( !tramp->initialize(new TrampolineShader()) ) { GLOG("ERROR: Could not initialize third entity\n"); return false; }
		tramp->loadTexture("Textures/trampoline.tga");
		tramp->setPosition(Vector3(-6.0f, 0.0f, -60.0f));
		tramp->setRoll(30.0f);
		tramp->addToDynamicWorld(m_dynamicsWorld);
		registerForSimulationTicks(tramp);
		m_graph->attach(tramp);

		// fourth trampoline
		tramp = new Trampoline(this, 10.0f, 15.0f);
		if ( !tramp->initialize(new TrampolineShader()) ) { GLOG("ERROR: Could not initialize fourth entity\n"); return false; }
		tramp->loadTexture("Textures/trampoline.tga");
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
		Vector3 startPoint = Vector3(-7.0f, cubeSize.y / 2.0f, -130.0f);
		CubeGroups::createCubePyramid(this, 8, startPoint, cubeSize, 0.3f, 0.0f);
		m_graph->popNode();
	}
	m_graph->popNode();

	// create the player and return result
	return resetPlayer();
}


void TestScene2::onUpdate(float dt)
{
	if (sphere_fracture != NULL)
		sphere_fracture->prepare(dt);
}

void TestScene2::onShutdown()
{
	if (sphere_fracture != NULL)
		delete sphere_fracture;
}