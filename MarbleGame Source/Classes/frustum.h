#ifndef BOGLGP_FRUSTUM_H
#define BOGLGP_FRUSTUM_H

#include "geom.h"

struct Plane {
	float a, b, c, d;
	float absa, absb, absc, absd;
};

class Frustum {
public:
	~Frustum();

	void updateFrustum();
	bool isSphereInFrustum(float x, float y, float z, float radius);
	bool isPointInFrustum(float x, float y, float z);
	bool isVertexInFrustum(Vertex *v);
	bool isAABBInFrustum(AABB *aabb);

	bool initialize();

private:
	Plane m_planes[6];

    enum {
	    PLANE_LEFT = 0,
	    PLANE_RIGHT,
	    PLANE_TOP,
	    PLANE_BOTTOM,
	    PLANE_FAR,
	    PLANE_NEAR
    };

	Plane extractPlane(float a, float b, float c, float d);
};

#endif