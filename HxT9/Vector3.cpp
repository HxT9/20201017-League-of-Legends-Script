#include "Vector3.h"
#include <cmath>
#include "globalVars.h"

Vector3::Vector3() {
	x = y = z = 0.f;
}
Vector3::Vector3(float px, float py, float pz) {
	x = px; y = py; z = pz;
}

float Vector3::getMagnitude() {
	return sqrt(pow(x, 2) + pow(z, 2));
}
void Vector3::normalize() {
	float mag = sqrt(pow(x, 2) + pow(z, 2));
	x /= mag;
	z /= mag;
}
Vector3 Vector3::normalized() {
	float mag = sqrt(pow(x, 2) + pow(z, 2));
	return Vector3(x / mag, y, z / mag);
}
Vector3 Vector3::setMagnitude(float mag) {
	float currentMag = sqrt(pow(x, 2) + pow(z, 2));
	float f = mag / currentMag;

	return { x * f, y, z * f };
}
Vector3 Vector3::setRelativeMagnitude(Vector3 origin, float mag) {
	Vector3 vDirection = Vector3(x - origin.x, y - origin.y, z - origin.z);
	float dx, dy, dz;
	Vector3 vDirection2 = vDirection.setMagnitude(mag);

	dx = vDirection2.x - vDirection.x; dy = vDirection2.y - vDirection.y; dz = vDirection2.z - vDirection.z;
	return { x + dx, y + dy, z + dz };
}
float Vector3::distTo(Vector3 pv) {
	return sqrt(pow(x - pv.x, 2) + pow(z - pv.z, 2));
}
float Vector3::getAngle() {
	return atan2f(z, x);
}
std::string Vector3::toString() {
	return "x: " + std::to_string(x) + ", y: " + std::to_string(y) + ", z: " + std::to_string(z);
}
bool Vector3::isDrawable() {
	return x > -1500 && x < 3500 && y > -1500 && y < 2500;
}
float Vector3::Dot(Vector3 vOther)
{
	return (x * vOther.x + z * vOther.z);
}
Vector3 Vector3::ProjectionOn(Vector3 vOther)
{
	Vector3 toProject = Vector3(x, y, z);
	float scale = (toProject.Dot(vOther)) / (vOther.Dot(vOther));
	return vOther * scale;
}
Vector3 Vector3::sameDirectionAs(Vector3 vOther)
{
	return vOther.normalized() * this->getMagnitude();
}
Equation Vector3::lineEquation(Vector3 vOther) {
	float slope = (vOther.z - z) / (vOther.x / x);
	float intercept = z - slope * x;
	return Equation(slope * -1, 1, intercept * -1);
}
Vector3 Vector3::rotatePoint(Vector3 pivot, float radAngle) {
	float s = sinf(radAngle);
	float c = cosf(radAngle);

	float x1 = x - pivot.x;
	float z1 = z - pivot.z;

	float x2 = x1 * c - z1 * s;
	float z2 = x1 * s + z1 * c;

	return Vector3(x2 + pivot.x, pivot.y, z2 + pivot.z);
}
Vector3 Vector3::setWorldY() {
	return Vector3(x, 60, z);
}
Vector3 Vector3::ToScreen() {
	Vector3 ret;
	GH.worldToScreen(this, &ret);
	return ret;
}