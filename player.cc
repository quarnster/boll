#include <math.h>

#include "player.h"

Player::Player() : Object() {
	jumpstart = 0;
	jumpplay = false;
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

void Player::update(Game *game) {
	maple_device_t *dev = maple_enum_dev(port, 0);
	float ballradius = 1.0f;
	bool inair = position.y <= -0.1f - ballradius;

	if (dev != NULL && dev->info.functions & MAPLE_FUNC_CONTROLLER) {
		cont_state_t* s = (cont_state_t*) maple_dev_status(dev);
		float cx = s->joyx / 4096.0f;
		float speed = s->joyy / 8192.0f;

		if (s->buttons & CONT_A) {
			if ((qtime - jumpstart) < 250) {
				direction.y = -0.85f;
			}
			if (!jumpplay && (qtime - jumpstart) < 125) {
				snd_sfx_play(sounds[JUMP], 15, 0);
				jumpplay = true;
			}
		} else {
			if (!inair) {
				jumpstart = qtime;
				jumpplay = false;
			}
		}

		rotation.y -= cx;

		direction.x += sin(rotation.y) * speed;
		direction.z += cos(rotation.y) * speed;
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

	q3dColorSet3f(&color, 0.0f, 1.0f, 0.0f);
	// do collision detection between spheres.
	// this sphere has allready been tested against the previous
	// spheres so we don't need to test with those again.
	for (int i = port+1; i < 4; i++) {
		Player *p = &game->player[i];
		Point3D v(position);
		v -= p->position;

		float dist = v.x * v.x + v.y * v.y + v.z * v.z;
		float minDist = 2 * ballradius;
		minDist *= minDist;

		if (dist < minDist) {
			float len = fsqrt(dist);

			// objects are colliding
			q3dColorSet3f(&color, 1.0f, 0.0f, 0.0f);
			q3dColorSet3f(&game->player[i].color, 1.0f, 0.0f, 0.0f);

			Point3D temp(direction);
			Point3D v2(v);

			v2 /= len;
			temp *= 0.5f;
			p->direction *= 0.5f;
			v2 /= 32.0f;

			direction = p->direction + v2 - temp;
			p->direction = temp - v2 - p->direction;

			len *= 2 * 2;
			v /= len;

			position += v;
			p->position -= v;

			snd_sfx_play(sounds[BOUNCE], 15, 0);
		} 
	}


	// TODO: proper collusion detection
	if (position.y > 0 - ballradius) {
		position.y = 0 - ballradius;
		float val = 0.75;

		if (direction.y < 1.0f) {
			val = direction.y * direction.y  * val;
		}
		direction.y = -direction.y * val;
		if (val < 0.1) direction.y = 0;
		snd_sfx_play(sounds[BOUNCE2], 15, 0);
	} else if (inair) {
		// airborn. do some gravity to the ball
		direction.y += 0.1f;
	}
}

void Player::draw() {
	sphere->_agl.z = rotation.z;
	sphere->_agl.x = rotation.x;

	q3dColorSetV(&sphere->material.color, &color);

	q3dMatrixLoad(&_q3dMatrixCamera);
	q3dMatrixTranslate(position.x, position.y, position.z);
	q3dMatrixStore(&_q3dMatrixTemp);

	q3dMatrixLoad(&_q3dMatrixPerspective);
	q3dMatrixApply(&_q3dMatrixTemp);

	q3dMatrixTransform(sphere->vertex, (pvr_vertex_t*)&sphere->_finalVertex[0].x, sphere->vertexLength, sizeof(pvr_vertex_t));
	q3dMatrixTransformNormals(sphere->_uVertexNormal, sphere->_vertexNormal, sphere->vertexLength);

	fillerPlayers.update(sphere);
	pvr_prim(&fillerPlayers.defaultHeader, sizeof(pvr_poly_hdr_t));
	fillerPlayers.draw(sphere);
}

