#include "nicks_scene.h"
#include "cube.h"
#include "cube_groups.h"
#include "platform.h"
#include "trampoline.h"
#include "cannon.h"
#include "fracture.h"
#include "conveyor.h"
#include "waypoint_types.h"


const int starting_checkpoint = 0; // 0 for start. 1-m for checkpoint 1-m where m is number of checkpoints


NickScene::NickScene() :
Scene(),
m_globeFracture(NULL)
{
}

bool NickScene::resetPlayer()
{
    if (m_player != NULL) {
        unregisterForSimulationTicks(m_player);
        m_player->destroy();
    }
    
	// create the player in the scene
	m_player = new Player(this, 1.0f);
	Color glowColor = Color(0.8f, 0.9f, 0.8f, 1.0f);
	if ( !m_player->initialize(new LightTexShader(1.2f, 0.1f, 0, glowColor, false))) {
		GLOG("ERROR: Could not initialize player entity\n");
		return false;
	}
    if (starting_checkpoint <= 0)
        m_player->setPosition(m_WPstart->getPosition() + Vector3(0.0f, 0.5f, 0.0f));
    else
        m_player->setPosition(m_allCheckpoints[starting_checkpoint-1]->getPosition() + Vector3(0.0f, 0.5f, 0.0f));
	m_player->setOrientation(270.0f, 20.0f, 0.0f);
	m_player->loadTexture("Textures/glass.tga");
	//m_player->setGlowColor(glowColor);
	m_player->addToDynamicWorld(m_dynamicsWorld);
	registerForSimulationTicks(m_player);
	s_Cam()->attachTo(m_player);
    m_graph->attach(m_player);
	return true;
}


bool NickScene::onInitialize()
{
    Vector3 nodeCenter = Vector3(0.0f, 0.0f, 0.0f);
    
	//-----------------------------------------------------------------
	// create the initial platform which has a button that user can hit
    // to turn the globe on
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
        nodeCenter = Vector3(0.0f, 100.0f, 300.0f);
        
        //start position Waypoint this is where the player starts/resets
        m_WPstart  = new Start(this, "Objects/goal.obj", 0.125f);
        if ( !m_WPstart->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
        m_WPstart->setPosition(nodeCenter + Vector3(0.0f, 0.0f, 10.0f));
        m_WPstart->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(m_WPstart);

		// create a platform
		Platform *platform = new Platform(this, Vector3(40.0f, 1.0f, 40.0f));
		if ( !platform->initialize(new LightShader(0.2f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize initial platform\n"); return false; }
		platform->setPosition(nodeCenter + Vector3(0.0f, -0.5f, 0.0f));
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);
    }
	m_graph->popNode();
	
    
    //-----------------------------------------------------------------
	// create long platform that has cannons shooting at it
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
        nodeCenter += Vector3(0.0f, -30.0f, -130.0f);
        
		// create a platform
		Platform *platform = new Platform(this, Vector3(10.0f, 1.0f, 220.0f));
		if ( !platform->initialize(new LightShader(0.3f, 1.0f, 8, RAND_LIGHT_COLOR))) { GLOG("WARNING: Could not initialize cannon platform\n"); }
		platform->setPosition(nodeCenter + Vector3(0.0f, 0.0f, 0.0f));
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);
        
        bool rightSide = false;
        for (int i = 0; i < 10; i++) {
            float cannonZ = (platform->getSize().z * 0.5f) - 50.0f - (i * 15.0f);
            float fireTime = 0.4f + RANDF(1.0f);
            
            // add a cannon aiming at the platform
            Cannon *cannon = new Cannon(this, Vector3(3.0f, 3.0f, 6.0f), 120.0f, fireTime);
            if (!cannon->initialize(new LightShader(0.1f, 1.0f, 8, Color(0.1f, 0.8f, 0.2f)))) { GLOG("WARNING: could not initialize first cannon\n"); }
            float cannonX = rightSide ? 20.0f : -20.0f;
            cannon->setPosition(nodeCenter + Vector3(cannonX, 10.0f, cannonZ));
            float yaw = rightSide ? 90.0f : -90.0f;
            cannon->setOrientation(yaw, -20.0f, 0.0f);
            cannon->addToDynamicWorld(m_dynamicsWorld);
            m_graph->attach(cannon);
            rightSide = !rightSide;
            
            m_cannons.push_back(cannon);
        }
        
        // create a button that will stop the cannons
        NickButton *stop_cannon_button = new NickButton(this, &NickScene::stopCannonsHit, Vector3(8.0f, 0.5f, 4.0f));
        if (!stop_cannon_button->initialize(new LightShader(0.3f, 1.0f, 8, RAND_COLOR))) { GLOG("ERROR: could not create stop_cannon_button\n"); }
		stop_cannon_button->setPosition(nodeCenter + Vector3(0.0f, stop_cannon_button->getSize().y, -105.0f));
		stop_cannon_button->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(stop_cannon_button);
    }
	m_graph->popNode();
    
    
    //-----------------------------------------------------------------
	// create platform after the cannon platform, which just has cube pyramid
    // that covers a gap to the next platform
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
        nodeCenter += Vector3(0.0f, 0.0f, -120.0f);
        
        // create a checkpoint now that user has gotten through cannons
        NickCheckPoint *cp = new NickCheckPoint(this, "Objects/goal.obj", 0.125f);
		if ( !cp->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
		cp->setPosition(nodeCenter + Vector3(8.0f, 0.5f, 0.0f));
		m_allCheckpoints.push_back(cp);
		cp->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(cp);
        cp->attachParticleSystem(new WaypointParticleSystem(200));
        
		// create a platform
		Platform *platform = new Platform(this, Vector3(50.0f, 1.0f, 12.0f));
		if ( !platform->initialize(new LightShader(0.2f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize gap platform\n"); return false; }
		platform->setPosition(nodeCenter + Vector3(25.0f, -0.5f, 0.0f));
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);
        
        // cube pyramid right before gap
        m_graph->pushNode();
		Vector3 cubeSize = Vector3(1.5f, 1.5f, 0.5f);
		Vector3 startPoint = nodeCenter + Vector3(49.0f, cubeSize.y / 2.0f, -5.0f);
		CubeGroups::createCubePyramid(this, 7, startPoint, cubeSize, 0.1f, 90.0f);
		m_graph->popNode();
        
        // create a second platform after gap
		platform = new Platform(this, Vector3(20.0f, 1.0f, 10.0f));
		if ( !platform->initialize(new LightShader(0.2f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize 2nd gap platform\n"); return false; }
		platform->setPosition(nodeCenter + Vector3(65.0f, -0.5f, 0.0f));
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);
    }
	m_graph->popNode();
    
    
    // create a conveyor to take player up to next section
    Conveyor *conveyor1 = new Conveyor(this, Vector3(10.0f, 1.0f, 80.0f), 2.5f );
    if ( !conveyor1->initialize(new LightTexScrollShader(0.3f, 1.0f, 4, Color(1.0f, 1.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f), conveyor1->getSpeed() * 0.5f, 3.0f))) { GLOG("ERROR: Could not initialize conveyor1\n"); return false; }
    conveyor1->setPosition(nodeCenter + Vector3(85.0f, 2.0f, -35.0f));
    conveyor1->setOrientation(180.0f, 5.0f, 0.0f);
    conveyor1->loadTexture("Textures/conveyor.tga");
    conveyor1->addToDynamicWorld(m_dynamicsWorld);
    m_graph->attach(conveyor1);
    
    
    //-----------------------------------------------------------------
	// create seemingly random small platform that user has to jump between
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
        nodeCenter += Vector3(85.0f, 5.0f, -85.0f);
        
        float lastZ = 0.0f;
        for (int i = 0; i < 20; i++) {
            Vector3 position = Vector3(-10.0f + RANDF(20.0f), -2.0f + RANDF(4.0f), lastZ - (5.0f + RANDF(5.0f)));
            
            Platform *platform = new Platform(this, Vector3(4.0f + RANDF(5.0f), 1.0f + RANDF(2.0f), 4.0f + RANDF(5.0f)));
            if ( !platform->initialize(new LightShader(0.3f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize sparse platform\n"); return false; }
            platform->setPosition(nodeCenter + position);
            platform->addToDynamicWorld(m_dynamicsWorld);
            m_graph->attach(platform);
            
            lastZ = position.z - platform->getSize().z * 0.5f;
        }
        
        nodeCenter.z += lastZ - 10.0f;
        
        // larger platform at the end of the small platforms
        Platform *platform = new Platform(this, Vector3(15.0f, 1.0f, 15.0f));
        if ( !platform->initialize(new LightShader(0.3f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize post-sparse platform\n"); return false; }
        platform->setPosition(nodeCenter + Vector3(0.0f, 0.0f, 0.0f));
        platform->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(platform);
        
        // create a checkpoint now that user has gotten through sparse platforms
        NickCheckPoint *cp = new NickCheckPoint(this, "Objects/goal.obj", 0.125f);
		if ( !cp->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
		cp->setPosition(nodeCenter + Vector3(0.0f, 0.5f, 0.0f));
		m_allCheckpoints.push_back(cp);
		cp->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(cp);
        cp->attachParticleSystem(new WaypointParticleSystem(200));
    }
    m_graph->popNode();
    
    
    //-----------------------------------------------------------------
    // create a slanted platform that drops down onto a large trampoline
    //-----------------------------------------------------------------
    m_graph->pushNode();
    {
        nodeCenter += Vector3(-25.0f, -5.0f, -10);
        
        // angled platform down to trampoline
        Platform *platform = new Platform(this, Vector3(30.0f, 1.0f, 15.0f));
        if ( !platform->initialize(new LightShader(0.3f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize slanted1 platform\n"); return false; }
        platform->setPosition(nodeCenter + Vector3(0.0f, 0.0f, 0.0f));
        platform->setOrientation(0.0f, 0.0f, -20.0f);
        platform->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(platform);
        
        // create large trampoline to bounce off
        Trampoline *tramp = new Trampoline(this, 80.0f, 50.0f);
        if ( !tramp->initialize(new TrampolineShader()) ) { GLOG("ERROR: Could not initialize large trampoline\n"); return false; }
        tramp->setPosition(nodeCenter + Vector3(-30.0f, -75.0f, 0.0f));
        tramp->loadTexture("Textures/trampoline.tga");
        tramp->addToDynamicWorld(m_dynamicsWorld);
        registerForSimulationTicks(tramp);
        m_graph->attach(tramp);
        
        // angled trampoline up on other side of trampoline
        platform = new Platform(this, Vector3(30.0f, 1.0f, 15.0f));
        if ( !platform->initialize(new LightShader(0.3f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize slanted2 platform\n"); return false; }
        platform->setPosition(nodeCenter + Vector3(-70.0f, -15.0f, 0.0f));
        platform->setOrientation(0.0f, 0.0f, 20.0f);
        platform->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(platform);
    }
    m_graph->popNode();
    
    
    //-----------------------------------------------------------------
    // create a row of trampolines that takes you towards the globe
    //-----------------------------------------------------------------
    m_graph->pushNode();
    {
        nodeCenter += Vector3(-105.0f, -10.0f, -20.0f);
        
        // platform before trampolines
        Platform *platform = new Platform(this, Vector3(20.0f, 1.0f, 50.0f));
        if ( !platform->initialize(new LightShader(0.3f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize pre-tramp platform\n"); return false; }
        platform->setPosition(nodeCenter + Vector3(0.0f, 0.0f, 0.0f));
        platform->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(platform);
        
        nodeCenter += Vector3(0.0f, 0.0f, -15.0f);
        
        // create 4 trampolines in a line at alternating angles
        float sign = -1.0f;
        for (int i = 0; i < 4; i++) {
            // first trampoline
            Trampoline *tramp = new Trampoline(this, 10.0f, 15.0f);
            if ( !tramp->initialize(new TrampolineShader()) ) { GLOG("ERROR: Could not initialize an angled tramp\n"); return false; }
            tramp->setPosition(nodeCenter + Vector3(sign * 6.0, 0.0f, -30.0f + (i * -15.0f)));
            sign *= -1.0f;
            tramp->setRoll(sign * 30.0f);
            tramp->loadTexture("Textures/trampoline.tga");
            tramp->addToDynamicWorld(m_dynamicsWorld);
            registerForSimulationTicks(tramp);
            m_graph->attach(tramp);
        }
        
        // platform after trampolines
        platform = new Platform(this, Vector3(50.0f, 1.0f, 20.0f));
        if ( !platform->initialize(new LightShader(0.3f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize after tramp platform\n"); return false; }
        platform->setPosition(nodeCenter + Vector3(-25.0f, 0.0f, -100.0f));
        platform->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(platform);
        
        // create a checkpoint now that user has gotten through trampolines
        NickCheckPoint *cp = new NickCheckPoint(this, "Objects/goal.obj", 0.125f);
		if ( !cp->initialize(new LightShader(0.1f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
		cp->setPosition(nodeCenter + Vector3(-10.0f, 1.0f, -100.0f));
		m_allCheckpoints.push_back(cp);
		cp->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(cp);
        cp->attachParticleSystem(new WaypointParticleSystem(200));
    }
    m_graph->popNode();
    
    
    //-----------------------------------------------------------------
    // create a row of trampolines that takes you towards the globe
    //-----------------------------------------------------------------
    m_graph->pushNode();
    {
        nodeCenter += Vector3(-50.0f, 0.0f, -170.0f);
        
        // create a conveyor to take player past fire breathers
        Conveyor *conveyor1 = new Conveyor(this, Vector3(15.0f, 1.0f, 100.0f), 2.5f );
        if ( !conveyor1->initialize(new LightTexScrollShader(0.3f, 1.0f, 4, Color(1.0f, 1.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f), conveyor1->getSpeed() * 0.5f, 3.0f))) { GLOG("ERROR: Could not initialize conveyor1\n"); return false; }
        conveyor1->setPosition(nodeCenter + Vector3(0.0f, 0.0f, 0.0f));
        conveyor1->setOrientation(180.0f, 0.0f, 0.0f);
        conveyor1->loadTexture("Textures/conveyor.tga");
        conveyor1->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(conveyor1);
        
        // create a fire breathing hydralisk
        FireBreather *fireBreather = new FireBreather(this, 15.0f, 5.0f, 2.0f);
        if ( !fireBreather->initialize(new LightTexShader(0.2f, 1.0, 8, Color(1.0f, 1.0f, 1.0f)))) { GLOG("ERROR: could not create fire breather\n"); return false; }
        fireBreather->setPosition(nodeCenter + Vector3(-20.0f, 5.0f, 15.0f));
        fireBreather->setOrientation(-90.0f, 0.0f, 0.0f);
        fireBreather->loadTexture("Textures/hydralisk.tga");
        fireBreather->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(fireBreather);
        fireBreather->attachParticleSystem(new FireParticleSystem(500));
        
        // create a fire breathing hydralisk
        fireBreather = new FireBreather(this, 15.0f, 5.0f, 2.0f);
        if ( !fireBreather->initialize(new LightTexShader(0.2f, 1.0, 8, Color(1.0f, 1.0f, 1.0f)))) { GLOG("ERROR: could not create fire breather\n"); return false; }
        fireBreather->setPosition(nodeCenter + Vector3(20.0f, 5.0f, -10.0f));
        fireBreather->setOrientation(90.0f, 0.0f, 0.0f);
        fireBreather->loadTexture("Textures/hydralisk.tga");
        fireBreather->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(fireBreather);
        fireBreather->attachParticleSystem(new FireParticleSystem(500));
    }
    m_graph->popNode();
    
    
    //-----------------------------------------------------------------
    // create platform that has cannon which shoots globe
    //-----------------------------------------------------------------
    m_graph->pushNode();
    {
        nodeCenter += Vector3(20.0f, 0.0f, -65.0f);
        
        // platform after fire breathers
        Platform *platform = new Platform(this, Vector3(15.0f, 1.0f, 30.0f));
        if ( !platform->initialize(new LightShader(0.3f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize fire platform\n"); return false; }
        platform->setPosition(nodeCenter + Vector3(0.0f, 0.0f, 0.0f));
        platform->setOrientation(30.0f, 0.0f, 0.0f);
        platform->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(platform);
        
        // create a button that will shoot the cannon at globe
        NickButton *globe_shoot_button = new NickButton(this, &NickScene::shootGlobeHit, Vector3(10.0f, 0.5f, 6.0f));
        if (!globe_shoot_button->initialize(new LightShader(0.3f, 1.0f, 8, RAND_COLOR))) { GLOG("ERROR: could not create globe_button\n"); return false; }
		globe_shoot_button->setPosition(nodeCenter + Vector3(5.0f, globe_shoot_button->getSize().y + 0.25f, -8.0f));
        globe_shoot_button->setOrientation(30.0f, 0.0f, 0.0f);
		globe_shoot_button->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(globe_shoot_button);
        
        m_globeCannon = new Cannon(this, Vector3(5.0f, 5.0f, 10.0f), 80.0f);
        if (!m_globeCannon->initialize(new LightShader(0.1f, 1.0f, 8, Color(0.1f, 0.8f, 0.2f)))) { GLOG("WARNING: could not initialize first cannon\n"); }
        m_globeCannon->setPosition(nodeCenter + Vector3(-0.0f, 2.0f, -40.0f));
        m_globeCannon->setOrientation(260.0f, 0.0f, 0.0f);
        m_globeCannon->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(m_globeCannon);
    }
    m_graph->popNode();
    
    //-----------------------------------------------------------------
	// create the ending platform that has the globe and waypoint on it
	//-----------------------------------------------------------------
    m_graph->pushNode();
    {
        nodeCenter = Vector3(0.0f, 30.0f, -600.0f);
        
        //// create a platform
		Platform *platform = new Platform(this, Vector3(70.0f, 2.0f, 70.0f));
		if ( !platform->initialize(new LightShader(0.2f, 1.0f, 4, RAND_COLOR))) { GLOG("ERROR: Could not initialize ending platform\n"); return false; }
        platform->setPosition(nodeCenter + Vector3(0.0f, 0.0f, 0.0f));
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);
        
        // create a fracture globe that explodes
        Sphere *globe = new Sphere(this, 20.0f);
        if ( !globe->initialize(new LightTexScrollShader(1.0f, 0.2f, 2, Color(1.0f, 1.0f, 1.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f), 0.001f))) { GLOG("ERROR: could not initialize globe.\n"); return false; }
        globe->setPosition(nodeCenter + Vector3(0.0f, 30.0f, 0.0f));
        globe->setOrientation(0.0f, -90.0f, 0.0f);
        globe->loadTexture("Textures/earth.tga");
        m_globeFracture = new FractureObject(globe, "Textures/voronoi_fracure.tga", 80.0f);
        if ( !m_globeFracture->addToScene(this) ) { GLOG("ERROR: could not add globe fracture to scene.\n"); return false; }
    }
    m_graph->popNode();
    
    
    // create a particle system
    ParticleSystem *m_particleSystem = new StarryParticleSystem(2000);
    m_particleSystem->startSystem();
    m_graph->attach(m_particleSystem);
    
    m_playerParticleSystem = new PlayerParticleSystem(300);
    m_playerParticleSystem->startSystem();
    m_graph->attach(m_playerParticleSystem);
    
    
	// create the player and return result
	return resetPlayer();
}

void NickScene::stopCannonsHit(NickButton *button) {
    // turn off all teh cannons by setting their refresh rate to infinity
    for (int i = 0; i < m_cannons.size(); i++) {
        m_cannons[i]->setFireTime(INFINITE_REFRESH_TIME);
    }
}

void NickScene::shootGlobeHit(NickButton *button) {
    // shoot cannon at the globe
    m_globeCannon->shootCannon();
    
    //// create a platform
    Platform *platform = new Platform(this, Vector3(30.0f, 1.0f, 30.0f));
    if ( !platform->initialize(new LightShader(0.2f, 1.0f, 4, RAND_COLOR))) { GLOG("ERROR: Could not initialize second to last platform\n"); }
    platform->setPosition(Vector3(-60.0f, 50.0f, -560.0f));
    platform->addToDynamicWorld(m_dynamicsWorld);
    m_graph->attach(platform);
    
    ParticleSystem *system = new RandomParticleSystem(400);
    system->moveToPoint(0.0f, 50.0f, -600.0f);
    system->speed = 4.0f;
    system->speedVar = 3.0f;
    system->startSystem();
    m_graph->attach(system);
    
    // create a checkpoint now that user has gotten through trampolines
    FinishCheckPoint *cp = new FinishCheckPoint(this, "Objects/goal.obj", 0.125f);
    if ( !cp->initialize(new LightShader(0.1f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n");}
    cp->setPosition(Vector3(0.0f, 32.0f, -600.0f));
    m_allCheckpoints.push_back(cp);
    cp->addToDynamicWorld(m_dynamicsWorld);
    m_graph->attach(cp);
}


void NickScene::onUpdate(float dt)
{
    if (m_globeFracture != NULL)
        m_globeFracture->prepare(dt);
    
    m_playerParticleSystem->moveToPoint(m_player->getPosition().x, m_player->getPosition().y, m_player->getPosition().z);
}

void NickScene::onRender()
{
}

void NickScene::onShutdown()
{
    if (m_globeFracture != NULL)
        delete m_globeFracture;
}