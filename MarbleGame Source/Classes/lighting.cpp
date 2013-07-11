#include "lighting.h"
#include "glslshader.h"
#include "camera.h"

// singleton getter for lighting
Lighting* s_Lighting()
{
	static Lighting s_lighting;
	return &s_lighting;
}

Lighting::Lighting() :
Light0Position(Vector3(0.0f, 10.0f, 0.0f))
{
	ECLight0Position = ECTransform(Light0Position, s_Cam()->modelviewMatrix());
}

void Lighting::update(float dt)
{
	ECLight0Position = ECTransform(Light0Position, s_Cam()->modelviewMatrix());
	ECCamPosition = ECTransform(s_Cam()->getPosition(), s_Cam()->modelviewMatrix());
}

void Lighting::applyLighting(GLSLProgram *program)
{
	program->sendUniform("ECLight0Position", ECLight0Position.x, ECLight0Position.y, ECLight0Position.z, 1.0f);
	program->sendUniform("ECCamPosition", ECCamPosition.x, ECCamPosition.y, ECCamPosition.z);
}

Vector3 Lighting::ECTransform(const Vector3& vec, Matrix4 *mat)
{
	Vector3 ECVec;
	kmVec3Transform(&ECVec, &vec, mat);
	return ECVec;
}