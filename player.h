#ifndef __INCLUDED_PLAYER_H
#define __INCLUDED_PLAYER_H

#include <q3d.h>
#include "object.h"
#include "resources.h"
#include "lib/QuatCamera.h"

class Game;

class Player : public Object {
public:
	Player();
	~Player();

	void setController(int port);
	void update(Game *game);
	void draw();

	void	addScore(int score);
	void	setScore(int score);
	int	getScore();
	int	getCurrentScore();

	QuatCamera camera;
/*
	float camzoom;
	float camheight;
	float camadd;
	float camagl;
*/
	float radius;
	bool active;

	q3dTypeColor color;
	q3dTypeColor baseColor;
private:
	int score;
	int scoreadd;

	int port;

	int previousButtons;
	int jumpstart;
	bool jumpplay;

	int thrusttime;
	bool thrusting;

	int dietime;

//	bool bice;

	q3dTypeQuaternion qRot;
};

#endif

