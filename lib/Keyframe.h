#ifndef __INCLUDED_Q3D_KEYFRAME_H
#define __INCLUDED_Q3D_KEYFRAME_H

#include "Point3D.h"

class KeyFrame : public Point3D {
public:
	float time;

	Point3D tangent;
/*
	float x;
	float y;
	float z;
*/
	float tension;
	float continuity;
	float bias;

	KeyFrame();
//	KeyFrame(float x, float y, float z, float time);
//	KeyFrame(float x, float y, float z, float tension, float continuity, float bias, float time);

//	void set(float x, float y, float z, float tension, float continuity, float bias, float time);

};


#endif
