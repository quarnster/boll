#include <stdlib.h>

#include "scoreobject.h"
#include "player.h"
#include "game.h"

ScoreObject::ScoreObject() : Object() {
	positionate();
}

void ScoreObject::positionate() {
	position.y = 2;

//	while (true) {
		position.x = ((rand() % 1000) / 1000.0f - 0.5f) * LEVELSIZE * 2;
		position.z = ((rand() % 1000) / 1000.0f - 0.5f) * LEVELSIZE * 2;

		// find out where in the level we are
		float px = (LEVELSIZE + position.x) / (2 * LEVELSIZE);
		float pz = (LEVELSIZE + position.z) / (2 * LEVELSIZE);

		int pos = (int) (pz * BOARDSIZE) *BOARDSIZE + (int) (px*BOARDSIZE);

		if (levelData[pos] != HOLE) {
//			break;
		} else if (levelData[pos] == HIGH) {
			position.y += 10;
//			break;
		}
//	}
}

void ScoreObject::update(Game *game) {
	for (int i = 0; i < 4; i++) {
		Player *p = &game->player[i];
		Point3D v(position);
		v -= p->position;

		float dist = v.x * v.x + v.y * v.y + v.z * v.z;
		float minDist = 3.0 + 1.0;
		minDist *= minDist;

		if (dist < minDist) {
			snd_sfx_play(sounds[MENU_SELECT], 255, 128);
			p->addScore(10);
			positionate();

			break;
		}
	}
	rotation.y += 0.05;
}

extern q3dTypePolyhedron *scorePolyhedron;
extern q3dTypeFiller fillerScore;

void ScoreObject::draw() {
//	scorePolyhedron->_agl.z = rotation.z;
//	scorePolyhedron->_agl.x = rotation.x;

	q3dColorSet3f(&scorePolyhedron->material.color, 1.0f, 1.0f, 0.0f);

	q3dMatrixLoad(&_q3dMatrixCamera);
	q3dMatrixTranslate(position.x, position.y, position.z);
	q3dMatrixRotateY(rotation.y);
//	q3dMatrixScale(0.5f, 0.5f, 0.5f);
	q3dMatrixStore(&_q3dMatrixTemp);

	q3dMatrixLoad(&_q3dMatrixPerspective);
	q3dMatrixApply(&_q3dMatrixTemp);

	q3dMatrixTransformPVR(scorePolyhedron->vertex, &scorePolyhedron->_finalVertex[0].x, scorePolyhedron->vertexLength, sizeof(pvr_vertex_t));
	q3dMatrixTransformNormals(scorePolyhedron->_uVertexNormal, scorePolyhedron->_vertexNormal, scorePolyhedron->vertexLength);

	fillerScore.update(scorePolyhedron);
	pvr_prim(&fillerScore.defaultHeader, sizeof(pvr_poly_hdr_t));
	fillerScore.draw(scorePolyhedron);
}

