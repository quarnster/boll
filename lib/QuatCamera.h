#ifndef __INCLUDE_QUATCAMERA_H
#define __INCLUDE_QUATCAMERA_H

#include <q3d.h>
#include "Point3D.h"

class QuatCamera {
public:
	Point3D		rotation;
	Point3D		position;
	float		zoom;

	QuatCamera();
	void toMatrix();

private:
//	q3dTypeQuaternion qx;
//	q3dTypeQuaternion qy;

	q3dTypeQuaternion qRot;

};

#endif

