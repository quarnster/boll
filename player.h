#ifndef __INCLUDED_PLAYER_H
#define __INCLUDED_PLAYER_H

#include "object.h"

class Player : public Object {
public:
	Player(int unit, int device);
	~Player();

	void update();

private:
	int port;
	int unit;

	bool jump;
	int jumpstart;
	bool jumpok;

};

#endif

