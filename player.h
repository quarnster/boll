#ifndef __INCLUDED_PLAYER_H
#define __INCLUDED_PLAYER_H

#include <q3d.h>
#include "object.h"
#include "game.h"

class Game;

class Player : public Object {
public:
	Player();
	~Player();

	void setController(int port);
	void update(Game *game);
	void draw();

	q3dTypeColor color;
private:
	int port;

	int previousButtons;
	int jumpstart;
	bool jumpplay;

	int dietime;

	bool bice;
};

#endif

