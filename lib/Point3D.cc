#include <kos.h>
#include "Point3D.h"

Point3D operator*(const float& mul, const Point3D& p) {
	Point3D tmp;
	tmp.x = mul * p.x;
	tmp.y = mul * p.y;
	tmp.z = mul * p.z;

	return tmp;
}

Point3D::Point3D() {
	x = 0;
	y = 0;
	z = 0;
}

Point3D::Point3D(const Point3D& p) {
	*this = p;
}

Point3D::Point3D(float x2, float y2, float z2) {
	x = x2;
	y = y2;
	z = z2;
}

Point3D::~Point3D() {
	x = 0;
	y = 0;
	z = 0;
}

void Point3D::normalize() {
	float len = fsqrt(x*x + y*y + z*z);
	x /= len;
	y /= len;
	z /= len;
}

void Point3D::set(float x2, float y2, float z2) {
	x = x2;
	y = y2;
	z = z2;
}

double Point3D::squaredLength() {
	return x*x + y*y + z*z;
}
double Point3D::length() {
	return fsqrt(squaredLength());
}

float Point3D::dot(const Point3D &p) const {
	return x * p.x + y * p.y + z * p.z;
}

Point3D Point3D::cross(const Point3D &p) const {
	Point3D tmp;
	tmp.x = y * p.z - z * p.y;
	tmp.y = z * p.x - x * p.z;
	tmp.z = x * p.y - y * p.x;
	return tmp;
}

void Point3D::operator=(const Point3D &p) {
	x = p.x;
	y = p.y;
	z = p.z;
}

Point3D Point3D::operator+(const Point3D &p) const {
	Point3D tmp = *this;
	tmp.x += p.x;
	tmp.y += p.y;
	tmp.z += p.z;

	return tmp;
}
Point3D Point3D::operator-(const Point3D &p) const {
	Point3D tmp = *this;
	tmp.x -= p.x;
	tmp.y -= p.y;
	tmp.z -= p.z;

	return tmp;
}

Point3D Point3D::operator*(const float& mul) {
	Point3D tmp = *this;
	tmp.x *= mul;
	tmp.y *= mul;
	tmp.z *= mul;

	return tmp;
}


Point3D Point3D::operator/(const float div) {
	Point3D tmp = *this;
	tmp.x /= div;
	tmp.y /= div;
	tmp.z /= div;

	return tmp;
}

Point3D Point3D::operator+=(const Point3D& p) {
	*this = *this + p;
	return *this;
}

bool Point3D::operator==(Point3D &p) const{
	return (x == p.x) && (y == p.y) && (z == p.z);
}

Point3D Point3D::operator*=(const float mul) {
	*this = *this * mul;
	return *this;
}

Point3D Point3D::operator/=(const float div) {
	*this = *this / div;
	return *this;
}

Point3D Point3D::operator-=(const Point3D& sub) {
	*this = *this - sub;
	return *this;
}

