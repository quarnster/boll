#ifndef __INCLUDED_GAME_H
#define __INCLUDED_GAME_H

#include <q3d.h>

#include "player.h"
#include "scoreobject.h"
#include "powerupobject.h"
#include "level.h"
#include "resources.h"

class Player;

#define MAX_SCORE_NUM 0
#define MAX_POWERUP_NUM 0

class Game {
public:
	Game();
	~Game();

	void run();

	uint32	frame;
	bool gameended;
	Player		*player;
	ScoreObject	score[MAX_SCORE_NUM];
	PowerupObject	powerup[MAX_POWERUP_NUM];
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
	uint32	soundtrack;
	bool done;
	bool endtrack;
};

#endif

