#ifndef __INCLUDED_PLAYER_H
#define __INCLUDED_PLAYER_H

#include <q3d.h>
#include "object.h"
#include "resources.h"

class Game;

class Player : public Object {
public:
	Player();
	~Player();

	void setController(int port);
	void update(Game *game);
	void draw();

	int score;

	float camzoom;
	float camheight;
	float camadd;
	float camagl;

	q3dTypeColor color;
private:
	int port;

	int previousButtons;
	int jumpstart;
	bool jumpplay;

	int thrusttime;
	bool thrusting;

	int dietime;

	bool bice;

	q3dTypeQuaternion qRot;
};

#endif

