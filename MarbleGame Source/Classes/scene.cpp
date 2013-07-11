#include "scene.h"
#include "scene_graph.h"
#include <iomanip>
#include "player.h"
#include "fracture.h"

#include "btBulletDynamicsCommon.h"
#include "BulletCollision/CollisionDispatch/btGhostObject.h"

#define BULLET_MULTITHREADED 0
#if BULLET_MULTITHREADED
#define __BT_SKIP_UINT64_H 1 // fixes a uint64_t redefinition error
#include "BulletMultiThreaded/Win32ThreadSupport.h"
#include "BulletMultiThreaded/SpuGatheringCollisionDispatcher.h"
#include "BulletMultiThreaded/SpuNarrowPhaseCollisionTask/SpuGatheringCollisionTask.h"
#include "BulletMultiThreaded/btParallelConstraintSolver.h"
#include "BulletMultiThreaded/SequentialThreadSupport.h"
#include "BulletMultiThreaded/PlatformDefinitions.h"
#endif

// physics tick callback
static void _tickCallback(btDynamicsWorld* world, btScalar timeStep) ;

Scene::Scene() :
m_wireframe(false),
	m_displayStats(false),
	m_FPS(0.0f),
	m_player(NULL),
	m_congratz(false)
{
	m_graph = new SceneGraph(this);
	//default glClearColor is Black
	m_clearColor_r = 0.0f;
	m_clearColor_g = 0.0f;
	m_clearColor_b = 0.0f;
}

Scene::~Scene()
{
	delete m_font;
    delete m_congratFont;
	delete m_graph;
	delete m_dynamicsWorld;
	delete m_solver;
	delete m_broadphase;
	delete m_dispatcher;
	delete m_collisionConfiguration;
}

void Scene::shutdown() { onShutdown(); }

bool Scene::initialize()
{
	srand((unsigned int)time(NULL));

	// intialize font used in this scene
	GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
	string fontVert = "Shaders/font.vert";
	string fontFrag = "Shaders/font.frag";
	m_font = new FreeTypeFont("Fonts/LiberationSans-Regular.ttf", viewport[2], viewport[3], 12, fontVert, fontFrag);
    if (!m_font->initialize()) {
        GLOG("Could not initialize the font\n");
		return false;
	}
    
    m_congratFont = new FreeTypeFont("Fonts/LiberationSans-Regular.ttf", viewport[2], viewport[3], 32, fontVert, fontFrag);
    if (!m_congratFont->initialize()) {
        GLOG("Could not initialize the congrat font\n");
		return false;
	}

#if BULLET_MULTITHREADED
	int maxNumOutstandingTasks = 4;

	// default collition configuration with max pool size set for multi-threaded
	btDefaultCollisionConstructionInfo cci;
	cci.m_defaultMaxPersistentManifoldPoolSize = 32768;
	m_collisionConfiguration = new btDefaultCollisionConfiguration(cci);

	// create multi-threaded dispatch
	btThreadSupportInterface* threadSupportCollision = new Win32ThreadSupport(Win32ThreadSupport::Win32ThreadConstructionInfo(
		"collision", processCollisionTask, createCollisionLocalStoreMemory, maxNumOutstandingTasks));
	m_dispatcher = new SpuGatheringCollisionDispatcher(threadSupportCollision, maxNumOutstandingTasks, m_collisionConfiguration);

	// create multi-threaded constraint solver
	Win32ThreadSupport::Win32ThreadConstructionInfo threadConstructionInfo(
		"solverThreads", SolverThreadFunc, SolverlsMemoryFunc, maxNumOutstandingTasks);
	Win32ThreadSupport* threadSupportSolver = new Win32ThreadSupport(threadConstructionInfo);
	threadSupportSolver->startSPU();
	m_solver = new btParallelConstraintSolver(threadSupportSolver);
	m_dispatcher->setDispatcherFlags(btCollisionDispatcher::CD_DISABLE_CONTACTPOOL_DYNAMIC_ALLOCATION);
#else
	// collision configuration contains default setup for memory, collision setup
	m_collisionConfiguration = new btDefaultCollisionConfiguration();

	// use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_dispatcher = new	btCollisionDispatcher(m_collisionConfiguration);

	// the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	m_solver = new btSequentialImpulseConstraintSolver;
#endif

	// btDbvtBroadphase is a good general purpose broadphase. Can also try out btAxis3Sweep.
	m_broadphase = new btDbvtBroadphase();
	m_broadphase->getOverlappingPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());

	// create the dynamics world and set some properties
	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration);
	m_dynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_USE_2_FRICTION_DIRECTIONS | SOLVER_RANDMIZE_ORDER;
	m_dynamicsWorld->getDispatchInfo().m_useContinuous = true;
	m_dynamicsWorld->setGravity(btVector3(0, -30, 0));
	m_dynamicsWorld->setInternalTickCallback((btInternalTickCallback)_tickCallback, this, false);

#if BULLET_MULTITHREADED
	m_dynamicsWorld->getSolverInfo().m_numIterations = 4;
	m_dynamicsWorld->getSolverInfo().m_solverMode |= SOLVER_SIMD | SOLVER_USE_WARMSTARTING;
	m_dynamicsWorld->getDispatchInfo().m_enableSPU = true;
#endif
    
    s_Cam()->setCameraMode(CM_3RD_PERSON);

	// initialize subclass
	bool inited = onInitialize();

	// check that player was created in subclass
	if (m_player == NULL) {
		GLOG("WARNING: subclass of scene should initialize m_player in the onInitialize method\n");
	}
    
	return inited;
}

void Scene::update(float dt)
{
    s_Mouse()->setMouseLocked(true);
    
	s_Cam()->update(dt);

	s_Lighting()->update(dt);

	m_graph->update(dt);

	onUpdate(dt);

	updateFPS(dt);

	if (s_Keyboard()->isKeyPressed('L')) { m_wireframe = !m_wireframe; }
	if (s_Keyboard()->isKeyPressed('T')) { m_displayStats = !m_displayStats; }
}

void Scene::stepPhysics(float dt)
{
	m_dynamicsWorld->stepSimulation(dt, 10);
}

void _tickCallback(btDynamicsWorld* world, btScalar timeStep)
{
	Scene *ptr = (Scene*)world->getWorldUserInfo();
	ptr->onSimulationTick(timeStep);
}

void Scene::onSimulationTick(float dt)
{
	for (EntityIterator entity = m_simulationTickEntities.begin(); entity != m_simulationTickEntities.end(); ++entity) {
		if ((*entity)->canBeRemoved()) {
			entity = m_simulationTickEntities.erase(entity);
			continue;
		}

		(*entity)->simulationTick(dt);
	}
    
    for (FractureIterator frac = m_simulationFractureObjects.begin(); frac != m_simulationFractureObjects.end(); ++frac) {
		(*frac)->simulationTick(dt);
	}
}

void Scene::render()
{
	// clear the screen
	glClearColor(m_clearColor_r, m_clearColor_g, m_clearColor_b, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set up face culling
	glEnable(GL_DEPTH_TEST);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);

#ifndef MARBLE_IOS_BUILD
	//set wireframe or not
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	if (m_wireframe)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif

	// set the light position to the players position
    if (m_player != NULL)
        s_Lighting()->setLight0Position(m_player->getPosition());

	// render the scene graph
	m_graph->render();
    
    // subclass render
    onRender();

#ifndef MARBLE_IOS_BUILD
	glDisable(GL_DEPTH_TEST);

	stringstream fpsMessage;
    fpsMessage << "FPS: " << std::setprecision(3) << m_FPS;
    m_font->printString(fpsMessage.str(), 20.0f, 20.0f);

	if(shouldShowScore() && m_player){
		stringstream pScore;
		pScore << "Score: " << m_player->getScore();
		m_font->printString(pScore.str(), 300.0f, 50.0f);
	}
    
    if (m_congratz) {
        float scale = s_Cam()->getScreenWidth() / 1024.0f;
		m_congratFont->printString("Congratulations!", 370.0f * scale, 600.0f * scale);
    }
	
	if (m_displayStats) {
		stringstream numInScene;     
		numInScene << "Entities in scene: " << m_graph->getNumEntitiesInScene();
		m_font->printString(numInScene.str(), 20.0f, 100.0f);

		stringstream numSceneCulled;     
		numSceneCulled << "Entities after scene graph culling: " << m_graph->getNumSceneGraphCulledEntities();
		m_font->printString(numSceneCulled.str(), 20.0f, 80.0f);

		stringstream numFrustumCulled;     
		numFrustumCulled << "Entities after frustum culling: " << m_graph->getNumFrustumCulledEntities();
		m_font->printString(numFrustumCulled.str(), 20.0f, 60.0f);

		stringstream numRendered;     
		numRendered << "Entities rendered: " << m_graph->getNumEntitiesRendered();
		m_font->printString(numRendered.str(), 20.0f, 40.0f);
	}
#endif
}


void Scene::updateFPS(float dt)
{
    static int frame = 0;
    static float fps = 0.0f;

    const float FPS_UPDATE_INTERVAL = 1.0;

    fps += dt;
    ++frame;
    if (fps > FPS_UPDATE_INTERVAL) 
    {
        m_FPS = float(frame) / fps;
        frame = 0;
        fps -= FPS_UPDATE_INTERVAL;
    } 
}