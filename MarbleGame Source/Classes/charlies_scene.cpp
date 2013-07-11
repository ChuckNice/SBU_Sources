#include "charlies_scene.h"
#include "scene_graph.h"
#include "cube.h"
#include "cube_groups.h"
#include "platform.h"
#include "player.h"
#include "trampoline.h"
#include "cannon.h"
#include "particle_types.h"
#include "OBJmodel.h"
#include "conveyor.h"
#include "waypoint_types.h"
#include "collectible.h"


const int starting_checkpoint = 0; // 0 for start. 1-m for checkpoint 1-m where m is number of checkpoints


CharlieScene::CharlieScene() :
Scene()
{
	setClearColor(0.5f, 0.5f, 0.5f);
}

bool CharlieScene::resetPlayer()
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
        m_player->setPosition(m_allCheckpoints[starting_checkpoint-1]->getPosition() + Vector3(0.0f, 0.5f, 0.0f)); // after cannons
	m_player->setYaw(270.0f);
	m_player->loadTexture("Textures/glass.tga");
	m_player->setGlowColor(glowColor);
	m_player->addToDynamicWorld(m_dynamicsWorld);
	registerForSimulationTicks(m_player);
	s_Cam()->attachTo(m_player);
    m_graph->attach(m_player);
	return true;
}


bool CharlieScene::onInitialize()
{
	//THE WORLD WILL GENERALLY PROGRESS IN Z DIRECTION WHILE OSCILLATING UP/DOWN and LEFT/RIGHT
    Vector3 nodeCenter = Vector3(0.0f, 0.0f, 0.0f);
    Vector3 prevNodeRefPoint = Vector3(0.0f, 0.0f, 0.0f);

	//-----------------------------------------------------------------
	// create the initial platform which has a waypoint and collectibles
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
        nodeCenter += Vector3(0.0f, 50.0f, 50.0f);
        
		// create a platform
		Platform *platform = new Platform(this, Vector3(40.0f, 1.0f, 40.0f));
		if ( !platform->initialize(new LightShader(0.2f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize initial platform\n"); return false; }
		platform->setPosition(nodeCenter + Vector3(0.0f, 0.0f, 0.0f));
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);

        //start position Waypoint this is where the player starts/resets
        m_WPstart  = new Start(this, "Objects/goal.obj", 0.125f);
        if ( !m_WPstart->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
        m_WPstart->setPosition(platform->getPosition() + Vector3(0.0f, 1.2f, 10.0f));
        m_WPstart->addToDynamicWorld(m_dynamicsWorld);
        m_graph->attach(m_WPstart);


		Collectible *coll_1 = new Collectible(this, "Objects/boxxy.obj", 100, 1.0f );
		if ( !coll_1->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create Collectible\n"); return false; }
		coll_1->setPosition(nodeCenter + Vector3(-5.0f, 3.0f, 0.0f));
		coll_1->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(coll_1);


		m_graph->pushNode();
		{

			bool right = false;
			float xPos, roll;
			float lastY = platform->getPosition().y - 1.0f;

			for( int i=0; i<5; i++){
				// trampoline
				Trampoline *tramp = new Trampoline(this, 10.0f, 10.0f);
				if ( !tramp->initialize(new TrampolineShader()) ) { GLOG("ERROR: Could not initialize first entity\n"); return false; }
				tramp->loadTexture("Textures/trampoline.tga");

				//decisions
				xPos = right ? platform->getPosition().x - 7.0f : platform->getPosition().x + 7.0f;
				roll = right ? 25.0f : -25.0f;	
				lastY += 6.0f;

				//doit
				tramp->setPosition(Vector3(xPos,  lastY, platform->getPosition().z - platform->getSize().z / 2 - 5.0f));
				tramp->setRoll(roll);
				tramp->addToDynamicWorld(m_dynamicsWorld);
				registerForSimulationTicks(tramp);
				m_graph->attach(tramp);
				right = !right;
			}
		}
		m_graph->popNode();

		//create a large spiral staircase up
		m_graph->pushNode();
		{
			OBJmodel *staircase = new OBJmodel(this, "Objects/spiral_staircase.obj", 0.5f, false, false, true);
			if ( !staircase->initialize(new LightShader(0.6f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
			staircase->setYaw(270.0f);
			staircase->setPosition(nodeCenter + Vector3(4.0f, staircase->getSize().y * 0.5f, 0.0));
			staircase->addToDynamicWorld(m_dynamicsWorld);
			m_graph->attach(staircase);
		}
		m_graph->popNode();


    }
	m_graph->popNode();
	
    
    //-----------------------------------------------------------------
	// create long platform at the top of the stacked tramps with collectibles and a checkpoint at the end
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
        nodeCenter += Vector3(0.0f, 32.0f, 7.0f);

		// create a platform
		Platform *platform = new Platform(this, Vector3(60.0f, 1.0f, 15.0f));
		if ( !platform->initialize(new LightShader(0.2f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize initial platform\n"); return false; }
		platform->setPosition(nodeCenter + Vector3(-20.0f, 0.0f, -20.0f));
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);
		
		// line of collectibles
		float yPos = platform->getPosition().y + 4.0f;
		float lastX = platform->getPosition().x + 10.0f;
		for( int i=0; i<5; i++){
			//string of collectibles
			Collectible *coll = new Collectible(this, "Objects/boxxy.obj", 100, 1.0f );
			if ( !coll->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create Collectible\n"); return false; }
			coll->setPosition(Vector3(lastX -= 3.0f, yPos, platform->getPosition().z));
			coll->addToDynamicWorld(m_dynamicsWorld);
			m_graph->attach(coll);
		}

		// Second line of collectibles
		lastX -= 7.0f;
		for( int i=0; i<5; i++){
			//string of collectibles
			Collectible *coll = new Collectible(this, "Objects/boxxy.obj", 100, 1.0f );
			if ( !coll->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create Collectible\n"); return false; }
			coll->setPosition(Vector3(lastX -= 3.0f, yPos, platform->getPosition().z));
			coll->addToDynamicWorld(m_dynamicsWorld);
			m_graph->attach(coll);
		}

		//and a checkpoint befor the drop
		Checkpoint *cp = new Checkpoint(this, "Objects/goal.obj", 0.125f);
		if ( !cp->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
		cp->setPosition(Vector3(lastX + 7.0f, platform->getPosition().y + 1.2f, platform->getPosition().z));
		m_allCheckpoints.push_back(cp);
		cp->addToDynamicWorld(m_dynamicsWorld);
		ParticleSystem *ps = new WaypointParticleSystem(300);
		cp->attachParticleSystem(ps);
		m_graph->attach(ps);
		m_graph->attach(cp);

		// update reference point
		prevNodeRefPoint = cp->getPosition();
    }
	m_graph->popNode();
    
    
	//-----------------------------------------------------------------
	// create 2 staggered platforms to drop down onto
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
		nodeCenter = prevNodeRefPoint + Vector3(0.0f, -16.0f, -15.0f);

		// create a platform
		Platform *platform = new Platform(this, Vector3(10.0f, 1.0f, 10.0f));
		if ( !platform->initialize(new LightShader(0.2f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize initial platform\n"); return false; }
		platform->setPosition(nodeCenter);
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);

		// verticle line of collectibles
		float lastY = platform->getPosition().y + 3.0f;
		for( int i=0; i<5; i++){
			//string of collectibles
			Collectible *coll = new Collectible(this, "Objects/boxxy.obj", 100, 1.0f );
			if ( !coll->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create Collectible\n"); return false; }
			coll->setPosition(Vector3(platform->getPosition().x , lastY += 3.0f, platform->getPosition().z));
			coll->addToDynamicWorld(m_dynamicsWorld);
			m_graph->attach(coll);
		}

		// create a platform
		platform = new Platform(this, Vector3(5.0f, 1.0f, 5.0f));
		if ( !platform->initialize(new LightShader(0.2f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize initial platform\n"); return false; }
		platform->setPosition(nodeCenter + Vector3(-5.0f, -16.0f, -15.0f));
		platform->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(platform);

		// verticle line of collectibles
		lastY = platform->getPosition().y + 3.0f;
		for( int i=0; i<5; i++){
			//string of collectibles
			Collectible *coll = new Collectible(this, "Objects/boxxy.obj", 100, 1.0f );
			if ( !coll->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create Collectible\n"); return false; }
			coll->setPosition(Vector3(platform->getPosition().x , lastY += 3.0f, platform->getPosition().z));
			coll->addToDynamicWorld(m_dynamicsWorld);
			m_graph->attach(coll);
		}

		// update reference point
		prevNodeRefPoint = platform->getPosition();
	}
	m_graph->popNode();


    //-----------------------------------------------------------------
	// create conveyor with raised platform and a cube pyramid and collectibles
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
		nodeCenter = prevNodeRefPoint + Vector3(0.0f, 0.0f, -46.0f);

		//// create a conveyor
		Conveyor *conveyor = new Conveyor(this, Vector3(15.0f, 1.0f, 80.0f), 20.0f );
		if ( !conveyor->initialize(new LightTexScrollShader(0.1f, 1.0f, 4, Color(1.0f, 1.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f), conveyor->getSpeed(), 3.0f))) { GLOG("ERROR: Could not initialize conveyor1\n"); return false; }
		conveyor->loadTexture("Textures/conveyor.tga");
		conveyor->setPosition(nodeCenter);
		conveyor->setYaw(180.0f);
		conveyor->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(conveyor);

		//random scattered collectibles
		for (int i = 0; i< 10; i++){
			Collectible *coll = new Collectible(this, "Objects/boxxy.obj", 100, 1.0f );
			if ( !coll->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create Collectible\n"); return false; }
			coll->setPosition(Vector3(conveyor->getPosition().x + RANDF(conveyor->getSize().x / 2) * RAND_NEG1_1, 
									conveyor->getPosition().y + RANDF(5.0f) + 1.0f, 
									conveyor->getPosition().z + RANDF(conveyor->getSize().z / 2) * RAND_NEG1_1));
			coll->addToDynamicWorld(m_dynamicsWorld);
			m_graph->attach(coll);
		}

		// update reference point = end of conveyor
		prevNodeRefPoint = conveyor->getPosition() + Vector3(0.0f, 0.0f, -conveyor->getSize().z/2);

    }
	m_graph->popNode();

	//-----------------------------------------------------------------
	// create platform with a pit and some more stuff on it
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
		nodeCenter = prevNodeRefPoint + Vector3(-60.0f, 0.0f, 0.0f);

		OBJmodel *pit = new OBJmodel(this, "Objects/big_platform.obj", 4.0f, false, false, true);
		if ( !pit->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
		pit->setYaw(270.0f);
		pit->setPosition(nodeCenter);
		pit->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(pit);

		// add a cube pyramid above pit
		//// add cube pyramid
		m_graph->pushNode();
		Vector3 cubeSize = Vector3(1.5f, 1.5f, 1.5f);
		Vector3 startPoint = Vector3(nodeCenter.x - 18.0f, nodeCenter.y + 15.0f, nodeCenter.z - 10.0f);
		CubeGroups::createCubePyramid(this, 15, startPoint, cubeSize, 0.2f, 90.0f);
		m_graph->popNode();

		// line of collectibles
		float yPos = startPoint.y - 25.0f;
		float lastZ = startPoint.z;
		float  xPos = startPoint.x;
		for( int i=0; i<5; i++){
			//string of collectibles
			Collectible *coll = new Collectible(this, "Objects/boxxy.obj", 100, 1.0f );
			if ( !coll->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create Collectible\n"); return false; }
			coll->setPosition(Vector3(xPos, yPos, lastZ += 3.0f));
			coll->addToDynamicWorld(m_dynamicsWorld);
			m_graph->attach(coll);
		}

		//and a checkpoint befor the drop
		m_WPgoal = new FinishCheckPoint(this, "Objects/goal.obj", 0.125f);
		if ( !m_WPgoal->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create OBJmodel\n"); return false; }
		m_WPgoal->setPosition(Vector3(pit->getPosition().x - pit->getSize().x/2 - 15.0f, pit->getPosition().y + 1.2f, pit->getPosition().z));
		m_allCheckpoints.push_back(m_WPgoal);
		m_WPgoal->addToDynamicWorld(m_dynamicsWorld);
		//ParticleSystem *ps = new WaypointParticleSystem(300);
		//m_WPgoal->attachParticleSystem(ps);
		//m_graph->attach(ps);
		m_graph->attach(m_WPgoal);


		//// add a cannon aiming at the cube group
		Cannon *cannon = new Cannon(this, Vector3(1.5f, 1.5f, 5.0f), 80.0f, -1.0f, 1.0f);
		if (!cannon->initialize(new LightShader(0.2f, 1.0f, 8, Color(0.1f, 0.8f, 0.2f)))) { GLOG("ERROR: could not initialize first cannon\n"); return false; }
		cannon->setPosition(nodeCenter + Vector3(30.0f, 10.0f, -30.0f));
		cannon->setOrientation(63.0f, 30.0f, 0.0f);
		cannon->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(cannon);

		//// add a button aiming to shoot the above cannon
		// this has the effect of covering valuable collectibles
		CannonButton *cannon_button = new CannonButton(this, Vector3(4.0f, 0.5f, 4.0f), 1.0f, cannon);
		if (!cannon_button->initialize(new LightShader(0.3f, 1.0f, 8, RAND_COLOR))) { GLOG("ERROR: could not create first cannon button\n"); return false; }
		cannon_button->setPosition(nodeCenter + Vector3(0.0f, pit->getSize().y/2 + (cannon_button->getSize().y * 0.5f) - 17.0f, 0.0f));
		cannon_button->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(cannon_button);

		//coll at top of ramp
		Collectible *coll = new Collectible(this, "Objects/boxxy.obj", 100, 1.0f );
		if ( !coll->initialize(new LightShader(0.2f, 1.0, 8, RAND_COLOR))) { GLOG("ERROR: could not create Collectible\n"); return false; }
		coll->setPosition(nodeCenter + Vector3(5.0f, pit->getSize().y/2 + (cannon_button->getSize().y * 0.5f) + 5.0f, -3.0f));
		coll->addToDynamicWorld(m_dynamicsWorld);
		m_graph->attach(coll);

		// update reference point
		prevNodeRefPoint = m_WPgoal->getPosition();

	}
	m_graph->popNode();

	
/*	//-----------------------------------------------------------------
	// TEMPLATE NODE
	//-----------------------------------------------------------------
	m_graph->pushNode();
	{
		nodeCenter = prevNodeRefPoint + Vector3(0.0f, 0.0f, -46.0f);
		// update reference point
		prevNodeRefPoint = conveyor->getPosition();
	}
	m_graph->popNode();
*/

	////// add cube pyramid
	//m_graph->pushNode();
	//Vector3 cubeSize = Vector3(1.5f, 1.5f, 0.5f);
	//Vector3 startPoint = Vector3(-6.0f, cubeSize.y / 2.0f, -11.0f);
	//CubeGroups::createCubePyramid(this, 6, startPoint, cubeSize, 0.2f);
	//m_graph->popNode();

	//// add cube pyramid
	//m_graph->pushNode();
	//Vector3 cubeSize = Vector3(1.5f, 1.5f, 0.5f);
	//Vector3 startPoint = Vector3(-71.0f, cubeSize.y / 2.0f, 9.0f);
	//CubeGroups::createCubePyramid(this, 7, startPoint, cubeSize, 0.2f, -90.0f);
	//m_graph->popNode();

	//// create a particle system
	//ParticleSystem *m_particleSystem = new StarryParticleSystem(2000);
	//m_particleSystem->startSystem();
	//m_graph->attach(m_particleSystem);

	//playerParticleSystem = new PlayerParticleSystem(200);
	//playerParticleSystem->startSystem();
	//m_graph->attach(playerParticleSystem);
      
	// create the player and return result
	return resetPlayer();
}


void CharlieScene::onUpdate(float dt)
{   
    //playerParticleSystem->moveToPoint(m_player->getPosition().x, m_player->getPosition().y, m_player->getPosition().z);
}

void CharlieScene::onRender()
{
}

void CharlieScene::onShutdown()
{

}



//void CharlieScene::stopCannonsHit(CharlieButton *button) {
//	// turn off all teh cannons by setting their refresh rate to infinity
//	for (int i = 0; i < m_cannons.size(); i++) {
//		m_cannons[i]->setFireTime(INFINITE_REFRESH_TIME);
//	}
//}


// create a platform
//Platform *platform = new Platform(this, Vector3(10.0f, 1.0f, 220.0f));
//if ( !platform->initialize(new LightShader(0.3f, 1.0f, 8, RAND_LIGHT_COLOR))) { GLOG("WARNING: Could not initialize cannon platform\n"); }
//platform->setPosition(nodeCenter + Vector3(0.0f, 0.0f, 0.0f));
//platform->addToDynamicWorld(m_dynamicsWorld);
//m_graph->attach(platform);
//      
//      bool rightSide = false;
//      for (int i = 0; i < 10; i++) {
//          float cannonZ = (platform->getSize().z * 0.5f) - 50.0f - (i * 15.0f);
//          float fireTime = 0.3f + RANDF(1.0f);
//          
//          // add a cannon aiming at the platform
//          Cannon *cannon = new Cannon(this, Vector3(3.0f, 3.0f, 6.0f), 120.0f, fireTime);
//          if (!cannon->initialize(new LightShader(0.1f, 1.0f, 8, Color(0.1f, 0.8f, 0.2f)))) { GLOG("WARNING: could not initialize first cannon\n"); }
//          float cannonX = rightSide ? 20.0f : -20.0f;
//          cannon->setPosition(nodeCenter + Vector3(cannonX, 10.0f, cannonZ));
//          float yaw = rightSide ? 90.0f : -90.0f;
//          cannon->setOrientation(yaw, -20.0f, 0.0f);
//          cannon->addToDynamicWorld(m_dynamicsWorld);
//          m_graph->attach(cannon);
//          rightSide = !rightSide;
//          
//          m_cannons.push_back(cannon);
//      }



//   //-----------------------------------------------------------------
//// create seemingly random small platform that user has to jump between
////-----------------------------------------------------------------
//m_graph->pushNode();
//{
//       nodeCenter = Vector3(85.0f, 75.0f, -35.0f);
//       
//       float lastZ = 0.0f;
//       for (int i = 0; i < 20; i++) {
//           Vector3 position = Vector3(-10.0f + RANDF(20.0f), -2.0f + RANDF(4.0f), lastZ - (5.0f + RANDF(5.0f)));
//           
//           Platform *platform = new Platform(this, Vector3(4.0f + RANDF(5.0f), 1.0f + RANDF(2.0f), 4.0f + RANDF(5.0f)));
//           if ( !platform->initialize(new LightShader(0.3f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize sparse platform\n"); return false; }
//           platform->setPosition(nodeCenter + position);
//           platform->addToDynamicWorld(m_dynamicsWorld);
//           m_graph->attach(platform);
//           
//           lastZ = position.z - platform->getSize().z * 0.5f;
//       }
//       
//       nodeCenter.z += lastZ - 10.0f;
//       
//       // larger platform at the end of the small platforms
//       Platform *platform = new Platform(this, Vector3(15.0f, 1.0f, 15.0f));
//       if ( !platform->initialize(new LightShader(0.3f, 0.8f, 8, RAND_COLOR))) { GLOG("ERROR: Could not initialize post-sparse platform\n"); return false; }
//       platform->setPosition(nodeCenter + Vector3(0.0f, 0.0f, 0.0f));
//       platform->addToDynamicWorld(m_dynamicsWorld);
//       m_graph->attach(platform);
//       
//   }
//   m_graph->popNode();


//// create a button that will light the globe
//CharlieButton *globe_button = new CharlieButton(this, &CharlieScene::globeButtonHit, Vector3(4.0f, 0.5f, 4.0f));
//if (!globe_button->initialize(new LightShader(0.3f, 1.0f, 8, RAND_COLOR))) { GLOG("ERROR: could not create globe_button\n"); return false; }
//globe_button->setPosition(nodeCenter + Vector3(0.0f, globe_button->getSize().y * 0.5f, -10.0f));
//globe_button->addToDynamicWorld(m_dynamicsWorld);
//m_graph->attach(globe_button);


//// create a button that will stop the cannons
//CharlieButton *stop_cannon_button = new CharlieButton(this, &CharlieScene::stopCannonsHit, Vector3(8.0f, 0.5f, 4.0f));
//if (!stop_cannon_button->initialize(new LightShader(0.3f, 1.0f, 8, RAND_COLOR))) { GLOG("ERROR: could not create stop_cannon_button\n"); }
//stop_cannon_button->setPosition(nodeCenter + Vector3(0.0f, stop_cannon_button->getSize().y, -105.0f));
//stop_cannon_button->addToDynamicWorld(m_dynamicsWorld);
//m_graph->attach(stop_cannon_button);

//void CharlieScene::globeButtonHit(CharlieButton *button)
//{
//    for (map<long, Fracture*>::iterator frac = m_globeFracture->getFractures()->begin(); frac != m_globeFracture->getFractures()->end(); ++frac) {
//        LightTexScrollShader *shader =  (LightTexScrollShader*)(*frac).second->getShaderProgram();
//        shader->ambientFactor = 1.0f;
//        shader->specularPower = 2;
//    }
//    
//    //-----------------------------------------------------------------
//	// create long platform that has cannons shooting at it
//	//-----------------------------------------------------------------
//	m_graph->pushNode();
//	{
//        Vector3 nodeCenter = Vector3(0.0f, 70.0f, 170.0f);
//        
//		// create a platform
//		Platform *platform = new Platform(this, Vector3(10.0f, 1.0f, 220.0f));
//		if ( !platform->initialize(new LightShader(0.3f, 1.0f, 8, RAND_COLOR))) { GLOG("WARNING: Could not initialize cannon platform\n"); }
//		platform->setPosition(nodeCenter + Vector3(0.0f, 0.0f, 0.0f));
//		platform->addToDynamicWorld(m_dynamicsWorld);
//		m_graph->attach(platform);
//        
//        bool rightSide = false;
//        for (int i = 0; i < 10; i++) {
//            float cannonZ = (platform->getSize().z * 0.5f) - 50.0f - (i * 15.0f);
//            float fireTime = 0.3f + RANDF(1.5f);
//            
//            // add a cannon aiming at the platform
//            Cannon *cannon = new Cannon(this, Vector3(3.0f, 3.0f, 6.0f), 120.0f, fireTime);
//            if (!cannon->initialize(new LightShader(0.1f, 1.0f, 8, Color(0.1f, 0.8f, 0.2f)))) { GLOG("WARNING: could not initialize first cannon\n"); }
//            float cannonX = rightSide ? 20.0f : -20.0f;
//            cannon->setPosition(nodeCenter + Vector3(cannonX, 10.0f, cannonZ));
//            float yaw = rightSide ? 90.0f : -90.0f;
//            cannon->setOrientation(yaw, -20.0f, 0.0f);
//            cannon->addToDynamicWorld(m_dynamicsWorld);
//            m_graph->attach(cannon);
//            rightSide = !rightSide;
//            
//            m_cannons.push_back(cannon);
//        }
//        
//        
//    }
//	m_graph->popNode();
//}

//
//// create a fracture globe that explodes
//Sphere *globe = new Sphere(this, 20.0f);
//if ( !globe->initialize(new LightTexScrollShader(1.0f, 0.2f, 2, Color(1.0f, 1.0f, 1.0f, 1.0f), Vector3(1.0f, 0.0f, 0.0f), 0.001f))) { GLOG("ERROR: could not initialize sphere.\n"); return false; }
//globe->setPosition(nodeCenter + Vector3(0.0f, 20.0f, 0.0f));
//globe->setOrientation(0.0f, -90.0f, 0.0f);
//globe->loadTexture("Textures/earth.tga");
//m_globeFracture = new FractureObject(globe, "Textures/voronoi_fracure.tga", 40.0f);
//if ( !m_globeFracture->addToScene(this) ) { GLOG("ERROR: could not add fracture to scene.\n"); return false; }