#ifndef __INCLUDED_TUNNEL_H
#define __INCLUDED_TUNNEL_H

#include <kos.h>

class Point;
class Tunnel {
public:
	Tunnel();
	~Tunnel();
	void draw();
private:
	pvr_poly_hdr_t header;
	Point	*p;
	int	bw;
	int	bh;
};

#endif

