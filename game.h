#ifndef __INCLUDED_GAME_H
#define __INCLUDED_GAME_H

#include <q3d.h>

#include "player.h"
#include "level.h"
#include "resources.h"

class Player;

class Game {
public:
	Game();
	~Game();

	void run();

	Player	*player;
private:
	void update();
	void draw();

	q3dTypeCamera cam;
	Level	level;

	pvr_poly_hdr_t	crossHeader;
//	Object	*objects;
};

#endif

