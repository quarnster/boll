#ifndef __INCLUDED_GAME_H
#define __INCLUDED_GAME_H

#include <q3d.h>

#include "player.h"
#include "level.h"

class Game {
public:
	Game();
	~Game();

	void update();
	void draw();

private:
	q3dTypeCamera cam;
	Player	player[4];
	Level	level;

//	Object	*objects;
};

#endif

