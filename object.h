#ifndef __INCLUDED_OBJECT_H
#define __INCLUDED_OBJECT_H

#include <q3d.h>

class Object {
public:
	Object();

	void update();

	q3dTypeVertex position;
	q3dTypeVertex direction;

private:
};

#endif

