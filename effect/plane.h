#ifndef __INCLUDED_PLANE_H
#define __INCLUDED_PLANE_H

#include <kos.h>

class Point;
class Plane {
public:
	Plane();
	~Plane();
	void draw();
private:
	pvr_poly_hdr_t header;
	Point	*p;
	int	bw;
	int	bh;
};

#endif

