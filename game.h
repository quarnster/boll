#ifndef __INCLUDED_GAME_H
#define __INCLUDED_GAME_H

#include <q3d.h>

#include "player.h"
#include "scoreobject.h"
#include "level.h"
#include "resources.h"

class Player;

#define MAX_SCORE_NUM 200

class Game {
public:
	Game();
	~Game();

	void run();

	bool gameended;
	Player		*player;
	ScoreObject	score[MAX_SCORE_NUM];
private:
	void reset();
	void update();
	void draw();

	q3dTypeCamera cam;
	Level	level;

	pvr_poly_hdr_t	crossHeader;
	pvr_poly_hdr_t	fadeHeader;
//	Object	*objects;
	uint64	gamestart;
	bool done;
};

#endif

