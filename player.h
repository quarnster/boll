#ifndef __INCLUDED_PLAYER_H
#define __INCLUDED_PLAYER_H

#include <q3d.h>
#include "object.h"

class Player : public Object {
public:
	Player();
	~Player();

	void setController(int port);
	void update();
	void draw(q3dTypeCamera *cam);

private:
	int port;

	int previousButtons;
	int jumpstart;

};

#endif

