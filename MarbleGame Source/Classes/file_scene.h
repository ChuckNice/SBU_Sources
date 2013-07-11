
#ifndef __FILE_SCENE__
#define __FILE_SCENE__

#include "global.h"

/**
 This class is a scene that is loaded from file
 */
class FileScene : public Scene
{
public:
	FileScene(const string& filePath);
	bool resetPlayer();
    
protected:
	bool onInitialize();
	void onUpdate(float dt);
    void onRender();
	void onShutdown();
    
private:
    string m_filePath;
};

#endif
