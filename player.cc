#include <math.h>

#include "player.h"

Player::Player() : Object() {
	jumpstart = 0;
	previousButtons = 0;
}

Player::~Player() {
}

void Player::setController(int port) {
	this->port = port;
}

extern q3dTypePolyhedron *sphere;
extern q3dTypeFiller fillerPlayers;
extern uint32 qtime;

void Player::update() {
	maple_device_t *dev = maple_enum_dev(port, 0);
	bool inair = position.y <= -0.1f;

	if (dev != NULL && dev->info.functions & MAPLE_FUNC_CONTROLLER) {
		cont_state_t* s = (cont_state_t*) maple_dev_status(dev);
		float cx = s->joyx / 4096.0f;
		float cy = s->joyy / 4096.0f;

		if (s->buttons & CONT_A) {
			if ((qtime - jumpstart) < 250) {
				direction.y = -0.85f;
			}
		} else {
			if (!inair)
				jumpstart = qtime;
		}

		direction.x += cx;
		direction.z += cy;
		direction.x = direction.x < -2 ? -2 : direction.x > 2 ? 2 : direction.x;
		direction.z = direction.z < -2 ? -2 : direction.z > 2 ? 2 : direction.z;

		previousButtons = s->buttons;
	}

	// Update position and stuff
	direction.x *= 0.97f;
	direction.z *= 0.97f;

	position.x += direction.x;
	position.y += direction.y;
	position.z += direction.z;

	rotation.x -= direction.x * 0.25;
	rotation.z += direction.z * 0.25;

	// TODO: proper collusion detection
	if (position.y > 0) {
		position.y = 0;
		float val = 0.75;

		if (direction.y < 1.0f) {
			val = direction.y * direction.y  * val;
		}
		direction.y = -direction.y * val;
		if (val < 0.1) direction.y = 0;
	} else if (inair) {
		// airborn. do some gravity to the ball
		direction.y += 0.1f;
	}
}

void Player::draw(q3dTypeCamera *cam) {
	sphere->_pos.x = position.x;
	sphere->_pos.y = position.y;
	sphere->_pos.z = position.z;

	sphere->_agl.z = rotation.z;
	sphere->_agl.x = rotation.x;

	sphere->material.header = fillerPlayers.defaultHeader;
	q3dColorSet3f(&sphere->material.color, 0.5f, 0.5f, 0.75f);
	q3dPolyhedronPaint(sphere, cam, &fillerPlayers);
}

