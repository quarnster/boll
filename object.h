#ifndef __INCLUDED_OBJECT_H
#define __INCLUDED_OBJECT_H

#include <q3d.h>
#include "lib/Point3D.h"
//#include "game.h"

class Game;
class Object {
public:
	virtual ~Object() {
	};
	virtual void update(Game *game) = 0;
	virtual void draw() = 0;

	Point3D		position;
	Point3D		rotation;
	Point3D		direction;

private:
};

#endif

