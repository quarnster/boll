#ifndef __INCLUDED_POWERUPOBJECT_H
#define __INCLUDED_POWERUPOBJECT_H

#include <q3d.h>
#include "object.h"

enum {
	PLUS50 = 0,
	MINUS50,
	GET_HIM,
	WATCH_OUT,
	TYPENUM
};

class PowerupObject : public Object {
public:
	PowerupObject();

	void	update(Game *game);
	void	draw();
	void	positionate();

private:
	uint64	appearTime;
	int	type;
};

#endif

