#include <math.h>
#include "level.h"

extern q3dTypeFiller fillerLevel;

Level::Level() {
	vertex = (q3dTypeVertex*) malloc(3*33*33*sizeof(q3dTypeVertex));
	finalVertex = (pvr_vertex_t*) malloc(3*33*33*sizeof(pvr_vertex_t));
	positions = (q3dTypeVertex*) malloc(32*32*sizeof(q3dTypeVertex));
	finalPositions = (q3dTypeVertex*) malloc(32*32*sizeof(q3dTypeVertex));

	int pos = 0;
	for (int z = 0; z < 33; z++) {
		float zpos = (z / 32.0f) * 2 - 1;
		zpos *= 10;
		for (int x = 0; x < 33; x++) {
			float xpos = (x / 32.0f) * 2 - 1;
			xpos *= 10;
			q3dVertexSet3f(&vertex[pos], xpos, 0, -zpos);
			pos++;
		}
	}

	pos = 0;
	for (int z = 0; z < 32; z++) {
		float zpos = (z / 31.0f) * 2 - 1 + 1;
		for (int x = 0; x < 32; x++) {
			float xpos = (x / 31.0f) * 2 - 1 + 1;
			q3dVertexSet3f(&positions[pos], xpos, 0, zpos);
			pos++;
		}
	}
}

Level::~Level() {
	if (vertex)
		free(vertex);
	if (finalVertex)
		free(finalVertex);
	if (positions)
		free(positions);
	if (finalPositions)
		free(finalPositions);
}

void Level::update() {
}

typedef struct {
	float a;
	float b;
	float c;
	float d;
} q3dTypePlane;

float q3dPlanePointInPlane(q3dTypePlane *plane, q3dTypeVector *point) {
	return plane->a * point->x + plane->b * point->y + plane->c * point->z + plane->d;
}
extern matrix_t projection_matrix;
extern matrix_t screen_matrix;
bool print = false;
bool pr = false;
bool press = false;
void Level::draw() {

	// transform vertices
	q3dMatrixLoad(&_q3dMatrixPerspective);
	q3dMatrixApply(&_q3dMatrixCamera);
	q3dMatrixStore(&_q3dMatrixTemp);
//	q3dMatrixTranslate(0,0,10);

	q3dMatrixTransform(vertex, (pvr_vertex_t*) &finalVertex[0].x, 3*33*33, sizeof(pvr_vertex_t));

//	q3dMatrixLoad(&_q3dMatrixPer
//	q3dMatrixLoad(&_q3dMatrixCamera);
//	q3dMatrixTransform(positions, &finalPositions[0].x, 32*32, sizeof(q3dTypeVertex));


	q3dTypePlane left;
	q3dTypePlane right;

//	q3dMatrixLoad(&projection_matrix);
	q3dMatrixLoad(&_q3dMatrixPerspective);
	q3dMatrixApply(&_q3dMatrixCamera);
	q3dMatrixStore(&_q3dMatrixTemp);

	left.a = _q3dMatrixTemp[3][0] + _q3dMatrixTemp[0][0];
	left.b = _q3dMatrixTemp[3][1] + _q3dMatrixTemp[0][1];
	left.c = _q3dMatrixTemp[3][2] + _q3dMatrixTemp[0][2];
	left.d = _q3dMatrixTemp[3][3] + _q3dMatrixTemp[0][3];

	right.a = _q3dMatrixTemp[3][0] - _q3dMatrixTemp[0][0];
	right.b = _q3dMatrixTemp[3][1] - _q3dMatrixTemp[0][1];
	right.c = _q3dMatrixTemp[3][2] - _q3dMatrixTemp[0][2];
	right.d = _q3dMatrixTemp[3][3] - _q3dMatrixTemp[0][3];

/*
	left.a *= 320;
	right.a *= 320;
	left.b *= 240;
	right.b *= 240;
*/
	MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)

		if (st->buttons & CONT_Y && !print) {
			print = true;
			printf("right.a: %f\n", right.a);
			printf("right.b: %f\n", right.b);
			printf("right.c: %f\n", right.c);
			printf("right.d: %f\n", right.d);
			printf("left.a: %f\n", left.a);
			printf("left.b: %f\n", left.b);
			printf("left.c: %f\n", left.c);
			printf("left.d: %f\n", left.d);

			printf("matrix:\n");
			printf("\t%f\t%f\t%f\t%f\n", _q3dMatrixTemp[0][0], _q3dMatrixTemp[0][1], _q3dMatrixTemp[0][2], _q3dMatrixTemp[0][3]);
			printf("\t%f\t%f\t%f\t%f\n", _q3dMatrixTemp[1][0], _q3dMatrixTemp[1][1], _q3dMatrixTemp[1][2], _q3dMatrixTemp[1][3]);
			printf("\t%f\t%f\t%f\t%f\n", _q3dMatrixTemp[2][0], _q3dMatrixTemp[2][1], _q3dMatrixTemp[2][2], _q3dMatrixTemp[2][3]);
			printf("\t%f\t%f\t%f\t%f\n", _q3dMatrixTemp[3][0], _q3dMatrixTemp[3][1], _q3dMatrixTemp[3][2], _q3dMatrixTemp[3][3]);
		} else if (!(st->buttons & CONT_Y)) {
			print = false;
		}
//		if (st->buttons & CONT_B && !press) {
//			pr = !pr;
//			press = true;
//		} else if (!(st->buttons & CONT_B)) {
//			press = false;
//		}
	MAPLE_FOREACH_END()


	
	// draw
	// todo...
	pvr_prim(&fillerLevel.defaultHeader, sizeof(pvr_poly_hdr_t));

	q3dTypeVector v;
	int pos = 0;
	for (int z = 0; z < 32; z++) { 
		for (int x = 0; x < 32; x++, pos++) {
//			if (finalPositions[pos].z < 0) {
//				continue;
//			}
			// todo: cull invisible stuff'
			int v1 = z * 33 + x;
			int v2 = v1 + 33;
			int v3 = v1 + 1;
			int v4 = v2 + 1;


			if (finalVertex[v1].z < 0 ||
				finalVertex[v2].z < 0 ||
				finalVertex[v3].z < 0 ||
				finalVertex[v4].z < 0
//				finalVertex[v1].z < 0.5 ||
//				finalVertex[v2].z < 0.5 ||
//				finalVertex[v3].z < 0.5 ||
//				finalVertex[v4].z < 0.5
				) {
					continue;
			}

			int color = 0xff00;

			v.x = finalVertex[v1].x;
			v.y = finalVertex[v1].y;
			v.z = finalVertex[v1].z;
			float l = q3dPlanePointInPlane(&left, &v);
			float r = q3dPlanePointInPlane(&right, &v);

			if (l < 0 /*|| r > 0*/) {
				color = 0xff0000;
			}

			finalVertex[v1].argb = color;
			finalVertex[v1].flags = PVR_CMD_VERTEX;
			pvr_prim(&finalVertex[v1], sizeof(pvr_vertex_t));

			finalVertex[v2].argb = color;
			finalVertex[v2].flags = PVR_CMD_VERTEX;
			pvr_prim(&finalVertex[v2], sizeof(pvr_vertex_t));

			finalVertex[v3].argb = color;
			finalVertex[v3].flags = PVR_CMD_VERTEX;
			pvr_prim(&finalVertex[v3], sizeof(pvr_vertex_t));

			finalVertex[v4].argb = color;
			finalVertex[v4].flags = PVR_CMD_VERTEX_EOL;
			pvr_prim(&finalVertex[v4], sizeof(pvr_vertex_t));
		}
	}
}

