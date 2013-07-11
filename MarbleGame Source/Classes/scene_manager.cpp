
#include "scene_manager.h"


// singleton getter for scene manager
SceneManager* s_SceneManager()
{
	static SceneManager s_sceneManager;
	return &s_sceneManager;
}


SceneManager::SceneManager() :
m_currentScene(NULL)
{
}


void SceneManager::shutdown()
{
    m_currentScene->shutdown();
    if (m_currentScene != NULL)
        delete m_currentScene;
}


bool SceneManager::loadScene(Scene *newScene)
{
    delete m_currentScene;
    
    m_currentScene = newScene;
    bool success = m_currentScene->initialize();

    return success;
}


void SceneManager::update(float dt)
{
    if (m_currentScene != NULL)
        m_currentScene->update(dt);
}


void SceneManager::stepPhysics(float dt)
{
    if (m_currentScene != NULL)
        m_currentScene->stepPhysics(dt);
}


void SceneManager::render()
{
    if (m_currentScene != NULL)
        m_currentScene->render();
}


