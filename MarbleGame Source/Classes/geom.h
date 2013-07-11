#ifndef BOGLGP_GEOM_H
#define BOGLGP_GEOM_H

/*
This header contains geometrix primitives such as TexCoord, Color,
Vec3, Mat4, and AABB. Some of these structs are extensions of kazmath
structs (like Vec3 and Mat4) which make common operations convenient
and simple.
*/

#include <cmath>
#include <kazmath/vec3.h>
#include <kazmath/mat4.h>
#include <kazmath/utility.h>
#include <float.h>
#include <vector>
#include "bullet3D/LinearMath/btTransform.h"
#include "bullet3D/LinearMath/btVector3.h"

inline float degToRad(const float degrees) {
	return degrees * kmPIOver180;
}


struct TexCoord
{
	float s, t;
	TexCoord(): s(0.0f), t(0.0f) {}
	TexCoord(float s, float t): s(s), t(t) { }
};


struct Color
{
	float r, g, b, a;
	Color(): r(0.0f), g(0.0f), b(0.0f), a(0.0f) { }
	Color(float R, float G, float B): r(R), g(G), b(B), a(1.0f) { }
	Color(float R, float G, float B, float A): r(R), g(G), b(B), a(A) { }
};


struct Vector3 : kmVec3
{
	Vector3() { kmVec3Fill(this, 0.0f, 0.0f, 0.0f); }
	Vector3(float X, float Y, float Z) { kmVec3Fill(this, X, Y, Z); }
	Vector3(const Vector3& v) { kmVec3Assign(this, &v); }
	Vector3(const kmVec3& kmv) { kmVec3Assign(this, &kmv); }
	Vector3(const btVector3& bvec) { kmVec3Fill(this, bvec.getX(), bvec.getY(), bvec.getZ()); }

	Vector3 operator+(const Vector3& v) const { return Vector3(x + v.x, y + v.y, z + v.z); }
	Vector3 operator-(const Vector3& v) const { return Vector3(x - v.x, y - v.y, z - v.z); }
	Vector3 operator*(const float s) const { return Vector3(x * s, y * s, z * s); }
	Vector3 operator/(const float s) const { return Vector3(x / s, y / s, z / s); }

	Vector3& operator+=(const Vector3& v) { kmVec3Add(this, this, &v); return *this; }
	Vector3& operator-=(const Vector3& v) { kmVec3Subtract(this, this, &v); return *this; }

	Vector3& operator=(const kmVec3& v) {
		if (this == &v) { return *this; }
		kmVec3Assign(this, &v);
		return *this;
	}

	float length() const { return kmVec3Length(this); }
	void normalize() { kmVec3Normalize(this, this); }
	float dot(const Vector3& v) { return kmVec3Dot(this, &v); }
	Vector3 cross(const Vector3& v) { Vector3 crossVec; kmVec3Cross(&crossVec, this, &v); return crossVec; }
	btVector3 btVec() const { return btVector3(x, y, z); }
};
typedef Vector3 Vec3;
typedef Vector3 Vertex;
typedef Vector3 Normal;


struct Matrix4 : kmMat4
{
	Matrix4() { kmMat4Identity(this); }
	Matrix4(const Matrix4& m) { kmMat4Assign(this, &m); }
	Matrix4(const kmMat4& kmm) { kmMat4Assign(this, &kmm); }
	Matrix4(const btTransform& btt) {
		float m[16];
		btt.getOpenGLMatrix(m);
		kmMat4Fill(this, m);
	}

	Matrix4& operator=(const kmMat4& m) {
		if (this == &m) { return *this; }
		kmMat4Assign(this, &m);
		return *this;
	}

	Matrix4& operator*=(const Matrix4& m) { kmMat4Multiply(this, this, &m); return *this; }

	void translatef(const float x, const float y, const float z) {
		kmMat4 transform;
		kmMat4Translation(&transform, x, y, z);
		kmMat4Multiply(this, this, &transform);
	}

	void rotatef(const float deg, const float x, const float y, const float z) {
		kmVec3 axis;
		kmVec3Fill(&axis, x, y, z);
		kmMat4 transform;
		kmMat4RotationAxis(&transform, &axis, degToRad(deg));
		kmMat4Multiply(this, this, &transform);
	}

	void scalef(const float scale) { scalef(scale, scale, scale); }
	void scalef(const float x, const float y, const float z) {
		kmMat4 transform;
		kmMat4Scaling(&transform, x, y, z);
		kmMat4Multiply(this, this, &transform);
	}

	// calculate normal matrix by taking 3x3 inverse transpose of 4x4 matrix
	std::vector<float> calculateNormalMatrix() 
	{
		//Grab the top 3x3 of the modelview matrix
		std::vector<float> M(3 * 3);
		M[0] = mat[0]; M[1] = mat[1]; M[2] = mat[2];
		M[3] = mat[4]; M[4] = mat[5]; M[5] = mat[6];
		M[6] = mat[8]; M[7] = mat[9]; M[8] = mat[10];

		//Work out the determinate
		float determinate = M[0] * M[4] * M[8] + M[1] * M[5] * M[6] + M[2] * M[3] * M[7];
		determinate -= M[2] * M[4] * M[6] + M[0] * M[5] * M[7] + M[1] * M[3] * M[8];

		//One division is faster than several
		float oneOverDet = 1.0f / determinate;

		std::vector<float> N(3 * 3);

		//Calculate the inverse and assign it to the transpose matrix positions
		N[0] = (M[4] * M[8] - M[5] * M[7]) * oneOverDet;
		N[3] = (M[2] * M[7] - M[1] * M[8]) * oneOverDet;
		N[6] = (M[1] * M[5] - M[2] * M[4]) * oneOverDet;

		N[1] = (M[5] * M[6] - M[3] * M[8]) * oneOverDet;
		N[4] = (M[0] * M[8] - M[2] * M[6]) * oneOverDet;
		N[7] = (M[2] * M[3] - M[0] * M[5]) * oneOverDet;

		N[2] = (M[3] * M[7] - M[4] * M[6]) * oneOverDet;
		N[5] = (M[1] * M[6] - M[8] * M[7]) * oneOverDet;
		N[8] = (M[0] * M[4] - M[1] * M[3]) * oneOverDet;

		return N;
	}
};
typedef Matrix4 Mat4;


struct AABB
{
	// layout of points is BFL, BFR, BBL, BBR, TFL, TFR, TBL, TBR 
	Vertex minVertex, maxVertex;

	AABB(): minVertex(Vertex(FLT_MAX, FLT_MAX, FLT_MAX)), maxVertex(Vertex(FLT_MAX, FLT_MAX, FLT_MAX)) { }
	AABB(const Vertex& inMin, const Vertex& inMax): minVertex(inMin), maxVertex(inMax) { }

	// get the radius of the AABB
	float getRadius() { return (maxVertex - minVertex).length() * 0.5f; }

	// adds a point to AABB by testing against current box and expanding box if point isn't inside
	void addPoint(const Vertex& newPoint) {
		// test against min point
		if (minVertex.x > newPoint.x || minVertex.x == FLT_MAX) { minVertex.x = newPoint.x; }
		if (minVertex.y > newPoint.y || minVertex.y == FLT_MAX) { minVertex.y = newPoint.y; }
		if (minVertex.z > newPoint.z || minVertex.z == FLT_MAX) { minVertex.z = newPoint.z; }

		// test against max point
		if (maxVertex.x < newPoint.x || maxVertex.x == FLT_MAX) { maxVertex.x = newPoint.x; }
		if (maxVertex.y < newPoint.y || maxVertex.y == FLT_MAX) { maxVertex.y = newPoint.y; }
		if (maxVertex.z < newPoint.z || maxVertex.z == FLT_MAX) { maxVertex.z = newPoint.z; }
	}

	// adds another AABB to this AABB. If this AABB needs to expand to fit the new one, it does so
	void addAABB(const AABB& newAABB, float padding = 0.0f) {
		// test min values
		if (minVertex.x > newAABB.minVertex.x || minVertex.x == FLT_MAX) { minVertex.x = newAABB.minVertex.x - padding; }
		if (minVertex.y > newAABB.minVertex.y || minVertex.y == FLT_MAX) { minVertex.y = newAABB.minVertex.y - padding; }
		if (minVertex.z > newAABB.minVertex.z || minVertex.z == FLT_MAX) { minVertex.z = newAABB.minVertex.z - padding; }

		// test max values
		if (maxVertex.x < newAABB.maxVertex.x || maxVertex.x == FLT_MAX) { maxVertex.x = newAABB.maxVertex.x + padding; }
		if (maxVertex.y < newAABB.maxVertex.y || maxVertex.y == FLT_MAX) { maxVertex.y = newAABB.maxVertex.y + padding; }
		if (maxVertex.z < newAABB.maxVertex.z || maxVertex.z == FLT_MAX) { maxVertex.z = newAABB.maxVertex.z + padding; }
	}

	// use this method to determine if this AABB is FULLY inside another AABB
	bool fullyInsideAABB(const AABB& testAABB) {
		// test min values
		if (minVertex.x < testAABB.minVertex.x) { return false; }
		if (minVertex.y < testAABB.minVertex.y) { return false; }
		if (minVertex.z < testAABB.minVertex.z) { return false; }

		// test max values
		if (maxVertex.x > testAABB.maxVertex.x) { return false; }
		if (maxVertex.y > testAABB.maxVertex.y) { return false; }
		if (maxVertex.z > testAABB.maxVertex.z) { return false; }

		return true;
	}
};


/** 
This struct represents a node in the scene graph. A node is an AABB bounding box
along with a parent node and a list of children nodes. By using the addChild method,
each nodes AABB will fully contain it's children's AABBs once the scene is created
*/
struct GraphNode
{
	AABB boundingBox;
	GraphNode *parent;
	std::vector<GraphNode*> children;
	bool canBeSeen;
	GraphNode(GraphNode *parentNode) : parent(parentNode), canBeSeen(false) { }
	~GraphNode() { for (unsigned int i = 0; i < children.size(); i++) { delete children[i]; } }

	void addChild(GraphNode *childNode) {
		boundingBox.addAABB(childNode->boundingBox);
		children.push_back(childNode);
	}
};


#endif
