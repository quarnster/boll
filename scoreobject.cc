#include <stdlib.h>

#include "scoreobject.h"
#include "player.h"
#include "game.h"

ScoreObject::ScoreObject() : Object() {
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
			p->score += 10;
			position.y = 2;
			position.x = ((rand() % 1000) / 1000.0f) * 64 - 32;
			position.z = ((rand() % 1000) / 1000.0f) * 64 - 32;

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

