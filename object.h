#ifndef __INCLUDED_OBJECT_H
#define __INCLUDED_OBJECT_H

#include <q3d.h>
#include "lib/Point3D.h"

class Object {
public:
	Object();

	void update();

	Point3D		position;
	Point3D		rotation;
	Point3D		direction;

private:
};

#endif

