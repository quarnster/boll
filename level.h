#ifndef __INCLUDED_LEVEL_H
#define __INCLUDED_LEVEL_H

#include <q3d.h>

enum {
	HOLE = 0,
	NORMAL,
	HIGH
};

#define LEVELSIZE 200
#define LEVELHEIGHT 10
extern uint8 levelData[];

class Level {
public:
	Level();
	~Level();

	void update();
	void draw();


private:
/*
	q3dTypeVertex	*vertex;
	q3dTypeVertex	*finalPositions;
	q3dTypeVertex	*positions;

	pvr_vertex_t	*finalVertex;
*/
};

#endif

