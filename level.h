#ifndef __INCLUDED_LEVEL_H
#define __INCLUDED_LEVEL_H

#include <q3d.h>

class Level {
public:
	Level();
	~Level();

	void update();
	void draw();


private:
	q3dTypeVertex	*vertex;
	q3dTypeVertex	*finalPositions;
	q3dTypeVertex	*positions;

	pvr_vertex_t	*finalVertex;
};

#endif

