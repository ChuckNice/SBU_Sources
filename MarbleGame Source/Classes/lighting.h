#ifndef __LIGHTING_H__
#define __LIGHTING_H__

#include "geom.h"

class GLSLProgram;

/**
This class is a singleton that manages lighting. When lights are changed/updated,
they are done so using this class. On each frame, entities that need lighting
will pass their shader programs into this class to have the lighting uniforms set
*/
class Lighting
{
public:
	Lighting();

	void update(float dt);
	void applyLighting(GLSLProgram *program);

	void setLight0Position(Vector3 pos) { Light0Position = pos; }

private:
	Vector3 Light0Position;
	Vector3 ECLight0Position;

	Vector3 ECCamPosition;

	Vector3 ECTransform(const Vector3& vec, Matrix4 *mat);
};

// external reference to lighting singleton
extern Lighting* s_Lighting();

#endif