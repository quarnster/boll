#include <math.h>

#include "player.h"

Player::Player() : Object() {
	jump = false;
	jumpstart = 0;
	jumpok = true;
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
	if (dev != NULL && dev->info.functions & MAPLE_FUNC_CONTROLLER) {
		cont_state_t* s = (cont_state_t*) maple_dev_status(dev);
		float cx = s->joyx / 4096.0f;
		float cy = s->joyy / 4096.0f;

		if (jumpok && s->buttons & CONT_A) {
			jump = true;
			if ((qtime - jumpstart) > 250) {
				jumpok = false;
			}
		} else {
			if (jump) {
				jump = false;
				jumpok = false;
			}
			jumpstart = qtime;
		}

		direction.x += cx;
		direction.z += cy;
		direction.x = direction.x < -2 ? -2 : direction.x > 2 ? 2 : direction.x;
		direction.z = direction.z < -2 ? -2 : direction.z > 2 ? 2 : direction.z;

	}

	// Update position and stuff
	direction.x *= 0.97f;
	direction.z *= 0.97f;

	if (!jumpok || !jump) {
		direction.y += 0.1f;
	} else if (jump) {
		if (direction.y < 0.1 && direction.y > -0.1)
			direction.y = -0.85f;
		else if (direction.y < -0.85f)
			direction.y += 0.05f;
	}


	position.x += direction.x;
	position.z += direction.z;
	position.y += direction.y;

	rotation.x -= direction.x * 0.25;
	rotation.z += direction.z * 0.25;

	// TODO: proper collusion detection
	if (position.y > 0) {
		position.y = 0;
		direction.y = -direction.y * 0.75;
	}
	if (position.y > -1 && fabs(direction.y) < 0.05) jumpok = true;
	else if (!jump) jumpok = false;
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

