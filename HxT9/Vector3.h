#pragma once
#include <string>
#include "Equation.h"
class Vector3
{
public:
	float x, y, z;

	Vector3();
	Vector3(float px, float py, float pz);
	
	inline Vector3 operator + (Vector3 pv) {
		return Vector3(x + pv.x, y + pv.y, z + pv.z);
	}
	inline Vector3 operator - (Vector3 pv) {
		return Vector3(x - pv.x, y - pv.y, z - pv.z);
	}
	inline Vector3 operator * (Vector3 pv) {
		return Vector3(x * pv.x, y * pv.y, z * pv.z);
	}
	inline Vector3 operator * (float pf) {
		return Vector3(x * pf, y * pf, z * pf);
	}
	inline Vector3 operator / (Vector3 pv) {
		return Vector3(x / pv.x, y / pv.y, z / pv.z);
	}
	inline bool operator == (Vector3 pv) {
		//return x == pv.x && y == pv.y && z == pv.z;
		return fabs(x - pv.x) < 0.000001 && fabs(y - pv.y) < 0.000001 && fabs(z - pv.z) < 0.000001;
	}
	inline bool operator != (Vector3 pv) {
		return fabs(x - pv.x) > 0.000001 || fabs(y - pv.y) > 0.000001 || fabs(z - pv.z) > 0.000001;
	}


	float getMagnitude();
	void normalize();
	Vector3 normalized();
	Vector3 setMagnitude(float mag);
	Vector3 setRelativeMagnitude(Vector3 origin, float mag);
	float distTo(Vector3 pv);
	float getAngle();
	std::string toString();
	bool isDrawable();
	float Dot(Vector3 vOther);
	Vector3 ProjectionOn(Vector3 vOther);
	Vector3 sameDirectionAs(Vector3 vOther);
	Equation lineEquation(Vector3 vOther);
	Vector3 rotatePoint(Vector3 pivot, float angle);
	float distTo2d(Vector3 pv);
};

