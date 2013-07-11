
#include "gui_scene.h"
#include "scene_graph.h"
#include "scene_manager.h"
#include "player.h"


GUIScene::GUIScene()
{
    
}


/*void GUIScene::update(float dt)
{
	Scene::update(dt);

	// user should be able to move mouse freely in a GUI scene
	s_Mouse()->setMouseLocked(false);

	onUpdate(dt);
}*/


void GUIScene::render()
{
	s_Mouse()->setMouseLocked(false);

    // clear the screen
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
	glEnable(GL_DEPTH_TEST);
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
    
    glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
    
    // subclass render
    onRender();
    
	//stringstream fpsMessage;
    //fpsMessage << "FPS: " << std::setprecision(3) << m_FPS;
    //m_font->printString(fpsMessage.str(), 20.0f, 20.0f);
}