#ifndef __INCLUDED_CREDITS_H
#define __INCLUDED_CREDITS_H

#include <kos.h>
#include <q3d.h>

class Credits {
public:
	Credits();
	~Credits();

	void run();

private:
	q3dTypeFiller cubeFiller;
	q3dTypeFiller sphereFiller;

	q3dTypePolyhedron *sphere;
	q3dTypePolyhedron *cube;
	q3dTypeCamera cam;

};

#endif

