#ifndef __CUBE_GROUPS__
#define __CUBE_GROUPS__

#include "cube.h"
#include "scene_graph.h"

class CubeGroups
{
public:
	// this function will add a pyramid of falling cubes to the scene.
	static bool createCubePyramid(Scene *scene, int levels, Vector3 startPoint, Vector3 cubeSize, float padding, float yaw = 0.0f)
	{
		Vector3 moveDir = s_Cam()->calculateLookDir(yaw, 0.0f);
		//Vector3 moveVec = moveDir * cubeSize.x / 2.0f;

		// add cube pyramid
		for (int i = 0; i < levels; i++) {
			for (int j = 0; j < (levels - i); j++) {
				Cube *cube = new Cube(scene, cubeSize);
				if ( !cube->initialize(new LightShader(0.2f, 1.0f, 8, Color(RANDF(1.0f), RANDF(1.0f), RANDF(1.0f), 0.95f))) ) {
					GLOG("ERROR: Could not initialize cube entity\n");
					return false;
				}
				Vector3 position = Vector3(startPoint.x + moveDir.x * float(j * cubeSize.x + (j * padding)), 
					startPoint.y + float(i * cubeSize.y), 
					startPoint.z + moveDir.z * float(j * cubeSize.x + (j * padding)));
				cube->setPosition(position);
				cube->setYaw(yaw);
				cube->addToDynamicWorld(scene->getDynamicsWorld());
				cube->getRigidBody()->forceActivationState(WANTS_DEACTIVATION);
				scene->getGraph()->attach(cube);
			}
			startPoint += moveDir;
		}
		return true;
	}
};

#endif