#ifndef __INCLUDED_LEVEL_H
#define __INCLUDED_LEVEL_H

#include <q3d.h>

class Level {
public:
	Level();
	~Level();

	void update();
	void draw(q3dTypeCamera *cam);

private:
	q3dTypeVertex	*vertex;
	pvr_vertex_t	*finalVertex;
};

#endif

