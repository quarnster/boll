#ifndef __INCLUDED_SCOREOBJECT_H
#define __INCLUDED_SCOREOBJECT_H

#include <q3d.h>
#include "object.h"

class ScoreObject : public Object {
public:
	ScoreObject();

	void update(Game *game);
	void draw();

private:
	void positionate();
};

#endif

