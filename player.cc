#include <stdlib.h>
#include <math.h>

#include "player.h"
#include "level.h"
#include "game.h"

Player::Player() : Object() {
	jumpstart = 0;
	jumpplay = false;
	previousButtons = 0;
	dietime = 0;

	thrusting = false;
	thrusttime = 0;

	camera.rotation.x = 0.55;
	camera.zoom = 22;

	q3dQuaternionInit(&qRot);

	active = false;
	radius = 3.0f;
}

Player::~Player() {
}

void Player::setController(int port) {
	this->port = port;
}

void Player::setScore(int s) {
	score = s;
	scoreadd = 0;
}

void Player::addScore(int s) {
	scoreadd += s;
}

int Player::getScore() {
	return score + scoreadd;
}

int Player::getCurrentScore() {
	return score;
}

extern q3dTypePolyhedron *sphere;
extern q3dTypeFiller fillerPlayers;
extern uint16 gametime;

void Player::update(Game *game) {

	maple_device_t *dev = maple_enum_dev(port, 0);
	float level = 0;

	// find out where in the level we are
	float px = (LEVELSIZE + position.x) / (2 * LEVELSIZE);
	float pz = (LEVELSIZE + position.z) / (2 * LEVELSIZE);

//	camadd = 0;
	int pos = (int) (pz * BOARDSIZE) *BOARDSIZE + (int) (px*BOARDSIZE);
	if (px < 0 || pz < 0 || px > 1 || pz > 1) {
		// outside of level
		q3dColorSet3f(&color, 1.0f, 0.0f, 0.0f);
		level = -100;
	} else if (levelData[pos] == HOLE) {
		// we are above a hole
		q3dColorSet3f(&color, 1.0f, 1.0f, 0.0f);
		level = -100;
	} else if (levelData[pos] == HIGH) {
		// we are at a cube
		level = 10;
		q3dColorSet3f(&color, 0.0f, 0.0f, 1.0f);
//		camadd = 10;
	} else {
		// solid ground
		q3dColorSet3f(&color, 0.0f, 1.0f, 0.0f);
	}
	bool inair = position.y >= level + 0.1f + radius;

	if (!game->gameended && !dietime && dev != NULL && dev->info.functions & MAPLE_FUNC_CONTROLLER) {
		cont_state_t* s = (cont_state_t*) maple_dev_status(dev);
		float cx = s->joyx / 2048.0f;
		float speed = -s->joyy / 3000.0f;

		if (s->buttons & CONT_DPAD_UP) {
			camera.rotation.x += 1.0f / 64.0f;
		} else if (s->buttons & CONT_DPAD_DOWN) {
			camera.rotation.x -= 1.0f / 64.0f;
		}

		if (s->buttons & CONT_DPAD_LEFT) {
			camera.zoom -= 1.0f / 4.0f;
		} else if (s->buttons & CONT_DPAD_RIGHT) {
			camera.zoom += 1.0f / 4.0f;
		}

		if (camera.zoom < 5) camera.zoom = 5;
		else if (camera.zoom > 40) camera.zoom = 40;

		if (camera.rotation.x < 0) camera.rotation.x = 0;
		else if (camera.rotation.x > 3.141592/2) camera.rotation.x = 3.141592/2;

		if (s->buttons & CONT_A) {
			if (jumpplay && (gametime - jumpstart) < 400) {
				direction.y = 0.65f;
			}
			if (!jumpplay && (gametime - jumpstart) < 125) {
				snd_sfx_play(sounds[JUMP], 255, 0);
				jumpplay = true;
			}
		} else {
			if (!inair) {
				jumpstart = gametime;
				jumpplay = false;
			}
		}

		if (s->buttons & CONT_X) {

			if (!thrusting) {
//				snd_sfx_play(sounds[JUMP], 255, 0);
				thrusting = true;
				thrusttime = gametime;
			}
		} else {
			if (thrusting/*  && (gametime - thrusttime) > 250*/) {
				thrusting = false;
				long max = gametime - thrusttime;
				if (max > 2000) max = 2000;
				speed = (max / 1000.0f) * 2.0f;
			}
		}

		rotation.y += cx;

		direction.x += sin(rotation.y) * speed;
		direction.z += cos(rotation.y) * speed;

		speed = s->ltrig / 5000.0f - s->rtrig/ 5000.0f;
		direction.x += sin(rotation.y-3.141592f*0.5) * speed;
		direction.z += cos(rotation.y-3.141592f*0.5) * speed;

//		direction.x = direction.x < -2 ? -2 : direction.x > 2 ? 2 : direction.x;
//		direction.z = direction.z < -2 ? -2 : direction.z > 2 ? 2 : direction.z;

		previousButtons = s->buttons;
	}

	// Update position and stuff
	direction.x *= 0.975f;
	direction.z *= 0.975f;

	position.x += direction.x;
	position.y += direction.y;
	position.z += direction.z;

	rotation.x -= direction.x * 0.25;
	rotation.z += direction.z * 0.25;

	float mul = 3.1415927 * 2 * 0.25 * 0.33;
	float a = -direction.z * mul * 0.5;
	static q3dTypeQuaternion qx;
	qx.w = cos(a);
	qx.x = sin(a);
	qx.y = qx.z = 0;

	a = direction.x * mul * 0.5;
	static q3dTypeQuaternion qz;
	qz.w = cos(a);
	qz.x = qz.y = 0;
	qz.z = sin(a);

	q3dQuaternionNormalize(&qx);
	q3dQuaternionNormalize(&qz);
/*
Qx = [ cos(a/2), (sin(a/2), 0, 0)]
Qy = [ cos(b/2), (0, sin(b/2), 0)]
Qz = [ cos(c/2), (0, 0, sin(c/2))]

And the final quaternion is obtained by Qx * Qy * Qz.
*/
	q3dQuaternionMul(&qx, &qz);
	q3dQuaternionMul(&qRot, &qx);


	camera.position.x = position.x;
	camera.position.z = position.z;
	camera.rotation.y = rotation.y;

	//	q3dColorSet3f(&color, 0.0f, 1.0f, 0.0f);
	// do collision detection between spheres.
	// this sphere has allready been tested against the previous
	// spheres so we don't need to test with those again.

	for (int i = port+1; i < 4; i++) {
		Player *p = &game->player[i];
		Point3D v(position);
		v -= p->position;

		float dist = v.x * v.x + v.y * v.y + v.z * v.z;
		float minDist = radius + p->radius;
		minDist *= minDist;

		if (dist < minDist) {
			float len = fsqrt(dist);

			// objects are colliding
			q3dColorSet3f(&color, 1.0f, 0.0f, 0.0f);
			q3dColorSet3f(&game->player[i].color, 1.0f, 0.0f, 0.0f);

			Point3D temp(direction);
			Point3D v2(v);

			Point3D normal = v;
			normal.normalize();
			Point3D reflection = direction - 2*(direction.dot(normal)) * normal;

			normal.x = -normal.x;
			normal.y = -normal.y;
			normal.z = -normal.z;
			Point3D reflection2 = p->direction - 2*(p->direction.dot(normal)) * normal;

			direction = reflection *0.25 - reflection2 * 0.75;
			p->direction = reflection2 * 0.25 - reflection *0.75;

			len *= 2 * 2;
			v /= len;

			position += v;
			p->position -= v;

			if (p->active) {
				active = true;
				p->active = false;
			} else if (active) {
				active = false;
				p->active = true;
			}
			snd_sfx_play(sounds[BOUNCE], 255, 0);
		} 
	}

	// Collide with boxes
	if (!dietime && position.y < 10 + radius) {
		px *= BOARDSIZE;
		pz *= BOARDSIZE;

		int ipx = (int) px;
		int ipz = (int) pz;

		// fractions
		float fpx = px - ipx;
		float fpz = pz - ipz;

		int c1 = fpx < 0.5 ? -1 : 1; // collide left/right
		int c2 = fpz < 0.5 ? -1 : 1; // collide top/bottom

		int pos1 = ipx + c1;
		int pos2 = ipz + c2;


		if (!(pos1 < 0 || pos1 >= BOARDSIZE)) {
			pos1 += ipz * BOARDSIZE;
			if (levelData[pos1] == HIGH && (fpx < 0.3 || fpx > 0.7)) {
				float add = fpx < 0.3 ? 0.31 : 0.69;
				position.x = (float) (ipx + add) / (float) BOARDSIZE * 2 * LEVELSIZE - LEVELSIZE;
				direction.x = -direction.x * 0.5;
			}
		}

		if (!(pos2 < 0 || pos2 >= BOARDSIZE)) {
			pos2 *= BOARDSIZE;
			pos2 += ipx;
			if (levelData[pos2] == HIGH && (fpz < 0.3 || fpz > 0.7)) {
				float add = fpz < 0.3 ? 0.31 : 0.69;
				position.z = ((float) (ipz + add) / (float) BOARDSIZE * 2 * LEVELSIZE - LEVELSIZE);
				direction.z = -direction.z * 0.5;
			}
		}
	}

	// TODO: proper collusion detection
	if (!dietime && position.y < level + radius) {
		position.y = level + radius;
		float val = 0.75;

		if (fabs(direction.y) < 1.0f) {
			val = direction.y * direction.y  * val;
		}
		direction.y = -direction.y * val;
		if (val < 0.1) direction.y = 0;
		snd_sfx_play(sounds[BOUNCE2], 255, 0);
	} else if (inair) {
		// airborn. do some gravity to the ball
		direction.y -= 0.075f;
	}

	if (dietime > 0 || (position.y < 0/* && level < 0*/)) {
		// we are falling into a hole or are outside of the level
		if (dietime == 0) {
			addScore(-5);
			if (active) {
				long lowest = 100000;
				int idx = 0;
				for (int i = 0; i < 4; i++) {
					int s = game->player[i].getScore();
					if (s < lowest) {
						idx = i;
						lowest = s;
					}
				}
				active = false;
				game->player[idx].active = true;
			}
			dietime = gametime;
			thrusting = false;
			snd_sfx_play(sounds[FALL], 255, 128);
		} else if (gametime - dietime > 1500) {
			dietime = 0;
			position.y = 4*radius;
			while (true) {
				position.x = ((rand() % 1000) / 1000.0f - 0.5f) * LEVELSIZE * 2;
				position.z = ((rand() % 1000) / 1000.0f - 0.5f) * LEVELSIZE * 2;

				// find out where in the level we are
				float px = (LEVELSIZE + position.x) / (2 * LEVELSIZE);
				float pz = (LEVELSIZE + position.z) / (2 * LEVELSIZE);

				int pos = (int) (pz * BOARDSIZE) *BOARDSIZE + (int) (px*BOARDSIZE);

				if (levelData[pos] == HIGH) {
					position.y += 10;
					break;
				} else if (levelData[pos] != HOLE) {
					break;
				}
			}
			direction.x = direction.y = direction.z = 0;
		}
	}

	color.r = baseColor.r;
	color.g = baseColor.g;
	color.b = baseColor.b;
	if (thrusting/*  && (gametime - thrusttime) > 250*/) {
		long max = gametime - thrusttime;
		if (max > 2000) max = 2000;
		float m = max / 2000.0f;
		float l = 0.5f + fsin((gametime / 1000.0f) * m * 3.141592*2) * 0.5f;
		color.r *= l;
		color.g *= l;
		color.b *= l;
	}

	if (scoreadd != 0 && game->frame % 10 == 1) {
		if (scoreadd < 0) {
			score--;
			scoreadd++;
		} else {
			score++;
			scoreadd--;
		}
	}

	if (!game->gameended && active) {
		if (game->frame % 30 == 1)
			scoreadd++;
		float l = 0.5f + fsin((gametime / 1000.0f)*3.141592 * 10) * 0.5f;
		color.r = l;
		color.g = l;
		color.b = l;
	}
}

void Player::draw() {
//	sphere->_agl.z = rotation.z;
//	sphere->_agl.x = rotation.x;
//	q3dTypeQuaternion q;
//	q3dTypeVector v;

//	q3dVectorSet3f(&v, 1, 0, 0);

	q3dColorSetV(&sphere->material.color, &color);

	q3dQuaternionToMatrix(&qRot);

	q3dMatrixLoad(&_q3dMatrixCamera);
	q3dMatrixTranslate(position.x, position.y, position.z);
	q3dMatrixScale(radius, radius, radius);

	q3dMatrixApply(&_q3dMatrixTemp);
	q3dMatrixStore(&_q3dMatrixTemp);

	q3dMatrixLoad(&_q3dMatrixPerspective);
	q3dMatrixApply(&_q3dMatrixTemp);

	q3dMatrixTransformPVR(sphere->vertex, &sphere->_finalVertex[0].x, sphere->vertexLength, sizeof(pvr_vertex_t));
//	q3dMatrixIdentity();
//	q3dMatrixRotateY(rotation.y);
//	q3dQuaternionToMatrix(&qRot);
//	q3dMatrixApply(&_q3dMatrixTemp);
//	q3dMatrixStore(&_q3dMatrixTemp);
//	q3dMatrixTransformNormals(sphere->_uVertexNormal, sphere->_vertexNormal, sphere->vertexLength);

	fillerPlayers.update(sphere);
	pvr_prim(&fillerPlayers.defaultHeader, sizeof(pvr_poly_hdr_t));
	fillerPlayers.draw(sphere);
}

