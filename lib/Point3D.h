#ifndef __INCLUDE_POINT3D_H
#define __INCLUDE_POINT3D_H

class Point3D {

public:
	float x;
	float y;
	float z;

	Point3D();
	Point3D(const Point3D&);
	Point3D(float x, float y, float z);
	~Point3D();

	void normalize();
	void set(float x, float y, float z);
	float dot(const Point3D&);

	bool operator==(Point3D&) const;
	void operator=(const Point3D&);
	Point3D operator+=(const Point3D&);
	Point3D operator+(const Point3D&);
	Point3D operator-(const Point3D&);
	Point3D operator-=(const Point3D&);
	Point3D operator*(const float);
	Point3D operator*=(const float);
	Point3D operator/(const float);
	Point3D operator/=(const float);
};


#endif
