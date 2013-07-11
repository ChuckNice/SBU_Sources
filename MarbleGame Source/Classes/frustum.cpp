
#include <cmath>
#include "frustum.h"
#include "camera.h"

bool Frustum::initialize()
{
	return true;
}

Frustum::~Frustum()
{
}

bool Frustum::isVertexInFrustum(Vertex *v) { return isPointInFrustum(v->x, v->y, v->z); }
bool Frustum::isPointInFrustum( float x, float y, float z )
{
	for (int p = 0; p < 6; p++)
    {
		if (m_planes[p].a * x + 
            m_planes[p].b * y + 
            m_planes[p].c * z + 
            m_planes[p].d < 0)
        {
			return false;
        }
    }
	return true;
}

bool Frustum::isSphereInFrustum(float x, float y, float z, float radius)
{
	for (int p = 0; p < 6; p++)
    {
		if (m_planes[p].a * x + 
            m_planes[p].b * y + 
            m_planes[p].c * z + 
            m_planes[p].d <= -radius)
        {
			return false;
        }
    }
	return true;
}

bool Frustum::isAABBInFrustum(AABB *aabb)
{
	Vertex min = aabb->minVertex;
	Vertex max = aabb->maxVertex;

	for (int p = 0; p < 6; p++)
    {
		// find the p-vertex (vertex that is "most in normals direction"
		Vertex p_vertex = min;
		if (m_planes[p].a >= 0) { p_vertex.x = max.x; }
		if (m_planes[p].b >= 0) { p_vertex.y = max.y; }
		if (m_planes[p].c >= 0) { p_vertex.z = max.z; }

		// test p-vertex against plane equation
		if (m_planes[p].a * p_vertex.x + 
			m_planes[p].b * p_vertex.y + 
			m_planes[p].c * p_vertex.z +
			m_planes[p].d < 0)
		{
			return false;
		}
	}
	return true;
}

void Frustum::updateFrustum()
{
	GLfloat *mvp = s_Cam()->modelviewProjectionMatrix()->mat;

	/* Extract the RIGHT plane */
	m_planes[PLANE_RIGHT] = extractPlane(mvp[ 3] - mvp[ 0],
	                                     mvp[ 7] - mvp[ 4],
	                                     mvp[11] - mvp[ 8],
	                                     mvp[15] - mvp[12]);

	/* Extract the LEFT plane */
	m_planes[PLANE_LEFT] = extractPlane(mvp[ 3] + mvp[ 0],
	                                    mvp[ 7] + mvp[ 4],
	                                    mvp[11] + mvp[ 8],
	                                    mvp[15] + mvp[12]);

	/* Extract the TOP plane */
	m_planes[PLANE_TOP] = extractPlane(mvp[ 3] - mvp[ 1],
	                                   mvp[ 7] - mvp[ 5],
	                                   mvp[11] - mvp[ 9],
	                                   mvp[15] - mvp[13]);

	/* Extract the BOTTOM plane */
	m_planes[PLANE_BOTTOM] = extractPlane(mvp[ 3] + mvp[ 1],
	                                      mvp[ 7] + mvp[ 5],
	                                      mvp[11] + mvp[ 9],
	                                      mvp[15] + mvp[13]);

	/* Extract the FAR plane */
	m_planes[PLANE_FAR] = extractPlane(mvp[ 3] - mvp[ 2],
	                                   mvp[ 7] - mvp[ 6],
	                                   mvp[11] - mvp[10],
	                                   mvp[15] - mvp[14]);

	/* Extract the NEAR plane */
	m_planes[PLANE_NEAR] = extractPlane(mvp[ 3] + mvp[ 2],
	                                   mvp[ 7] + mvp[ 6],
	                                   mvp[11] + mvp[10],
	                                   mvp[15] + mvp[14]);
}

Plane Frustum::extractPlane(float a, float b, float c, float d)
{
	float t = sqrt( a * a + b * b + c * c );
	Plane p;
	p.a = a / t;
	p.b = b / t;
	p.c = c / t;
	p.d = d / t;
	p.absa = fabsf(p.a);
	p.absb = fabsf(p.b);
	p.absc = fabsf(p.c);
	p.absd = fabsf(p.d);
	return p;
}