#include <stdlib.h>
#include <math.h>

#include "powerupobject.h"
#include "player.h"
#include "game.h"

PowerupObject::PowerupObject() : Object() {
//	positionate();
	type = WATCH_OUT;
	appearTime = 0;
}

void PowerupObject::positionate() {
	position.y = 2;

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
	appearTime = timer_ms_gettime64() + 10000;
}

void PowerupObject::update(Game *game) {
	if (appearTime > timer_ms_gettime64()) return;
	if (appearTime != 0) {
		snd_sfx_play(sounds[POWERUP_APPEAR], 255, 128);
		appearTime = 0;
	}

	for (int i = 0; i < 4; i++) {
		Player *p = &game->player[i];
		Point3D v(position);
		v -= p->position;

		float dist = v.x * v.x + v.y * v.y + v.z * v.z;
		float minDist = 3.0 + 1.0;
		minDist *= minDist;

		if (dist < minDist) {

			switch (type) {
				case PLUS50:
					p->addScore(50); break;
				case MINUS50:
					p->addScore(-50); break;
				case GET_HIM:
					snd_sfx_play(sounds[POWERUP_GET2], 255, 128);
					game->type = type;
					type = WATCH_OUT;
					break;
				case WATCH_OUT:
					snd_sfx_play(sounds[POWERUP_GET1], 255, 128);
					game->type = type;
					type = GET_HIM;
					break;
			}

			positionate();

			break;
		}
	}
	rotation.y -= 0.05;
}

extern q3dTypePolyhedron *powerupPolyhedron;
extern q3dTypeFiller fillerPowerup;
extern uint32 gametime;

void PowerupObject::draw() {
	if (appearTime > timer_ms_gettime64()) return;
//	powerupPolyhedron->_agl.z = rotation.z;
//	powerupPolyhedron->_agl.x = rotation.x;

	q3dColorSet3f(&powerupPolyhedron->material.color, 1.0f, 0.25f, 0.25f);

	q3dMatrixLoad(&_q3dMatrixCamera);
	q3dMatrixTranslate(position.x, position.y + fsin(gametime*0.005), position.z);
	q3dMatrixRotateY(rotation.y);
//	q3dMatrixScale(0.5f, 0.5f, 0.5f);
	q3dMatrixStore(&_q3dMatrixTemp);

	q3dMatrixLoad(&_q3dMatrixPerspective);
	q3dMatrixApply(&_q3dMatrixTemp);

	q3dMatrixTransformPVR(powerupPolyhedron->vertex, &powerupPolyhedron->_finalVertex[0].x, powerupPolyhedron->vertexLength, sizeof(pvr_vertex_t));
	q3dMatrixTransformNormals(powerupPolyhedron->_uVertexNormal, powerupPolyhedron->_vertexNormal, powerupPolyhedron->vertexLength);

	fillerPowerup.update(powerupPolyhedron);
	pvr_prim(&fillerPowerup.defaultHeader, sizeof(pvr_poly_hdr_t));
	fillerPowerup.draw(powerupPolyhedron);
}

