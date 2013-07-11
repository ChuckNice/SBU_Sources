#include "loading_scene.h"

LoadingScene::LoadingScene() :
Scene(),
	m_loadingFont(NULL)
{
}

bool LoadingScene::onInitialize()
{
	// intialize font used in this scene
	GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
	string fontVert = "Shaders/font.vert";
	string fontFrag = "Shaders/font.frag";
	m_loadingFont = m_loadingFont = new FreeTypeFont("Fonts/LiberationSans-Regular.ttf", viewport[2], viewport[3], 18, fontVert, fontFrag);
    if (!m_loadingFont->initialize()) {
        GLOG("Could not initialize the loading font\n");
		return false;
    }

	return true;
}

void LoadingScene::onShutdown()
{
	delete m_loadingFont;
}

void LoadingScene::render()
{
	glDisable(GL_DEPTH_TEST);
    m_loadingFont->printString("Loading Level...", 20.0f, 20.0f);
}