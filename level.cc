#include <math.h>
#include "level.h"

extern q3dTypeFiller fillerLevel;

#define NUM 33
#define NUM1 32
static q3dTypeVertex vertex[2*NUM*NUM] __attribute__((aligned(NUM1)));
static q3dTypeVertex vertexScreen[2*NUM*NUM] __attribute__((aligned(NUM1)));
static q3dTypeVertex position[NUM1*NUM1] __attribute__((aligned(NUM1)));
static q3dTypeVertex positionWorld[NUM1*NUM1] __attribute__((aligned(NUM1)));


Level::Level() {
	int pos = 0;
	for (int z = 0; z < NUM; z++) {
		float zpos = (z / (float) NUM1) * 2 - 1;
		zpos *= 10;
		for (int x = 0; x < NUM; x++) {
			float xpos = (x / (float) NUM1) * 2 - 1;
			xpos *= 10;
			vertex[pos].x = xpos;
			vertex[pos].y = 0;
			vertex[pos].z = -zpos;
			pos++;
		}
	}

	pos = 0;
	for (int z = 0; z < NUM1; z++) {
		float zpos = (vertex[z * NUM].z + vertex[z * NUM + NUM].z) / 2.0f;;
		for (int x = 0; x < NUM1; x++) {
			float xpos = (vertex[x].x + vertex[x+1].x) / 2.0f;;
			q3dVertexSet3f(&position[pos], xpos, 0, zpos);
			pos++;
		}
	}
}

Level::~Level() {
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

	q3dMatrixIdentity();


	q3dMatrixLoad(&projection_matrix);
	q3dMatrixApply(&_q3dMatrixCamera);
//	q3dMatrixLoad(&screen_matrix);
//	q3dMatrixApply(&projection_matrix);
//	q3dMatrixApply(&_q3dMatrixCamera);
	mat_transform(position, positionWorld, NUM1*NUM1, sizeof(q3dTypeVertex));

	// transform vertices
//	q3dMatrixLoad(&_q3dMatrixPerspective);
	q3dMatrixLoad(&screen_matrix);
	q3dMatrixApply(&projection_matrix);
	q3dMatrixApply(&_q3dMatrixCamera);

	mat_transform(vertex, vertexScreen, 2*NUM*NUM, sizeof(q3dTypeVertex));
//	q3dMatrixTransform(vertex, (pvr_vertex_t*) &finalVertex[0].x, 3*NUM*NUM, sizeof(pvr_vertex_t));

//	q3dMatrixLoad(&_q3dMatrixPer
//	q3dMatrixLoad(&_q3dMatrixCamera);
//	q3dMatrixTransform(positions, &finalPositions[0].x, NUM1*NUM1, sizeof(q3dTypeVertex));

	q3dTypePlane left;
	q3dTypePlane right;
	q3dTypePlane near;
	q3dTypePlane far;

	q3dMatrixIdentity();
	q3dMatrixLoad(&projection_matrix);
	q3dMatrixApply(&_q3dMatrixCamera);

//	q3dMatrixLoad(&_q3dMatrixCamera);
//	q3dMatrixLoad(&projection_matrix);
//	q3dMatrixApply(&_q3dMatrixCamera);
	q3dMatrixApply(&projection_matrix);
	q3dMatrixStore(&_q3dMatrixTemp);

	float project_scale = 1.0f;
	float SX = 320;
	float SY = 240;
	float angle_horizontal =  1.0; //atan2(SX/2,project_scale)-0.0001;
	float angle_vertical   =  1.0; //atan2(SY/2,project_scale)-0.0001;
	float sh               =  fsin(angle_horizontal);
	float sv               =  fsin(angle_vertical);
	float ch               =  fcos(angle_horizontal);
	float cv               =  fcos(angle_vertical);
	// left
	left.a = 1; //ch;
	left.b = 0;
	left.c = 0; //sh;
	left.d = 0;
/*
	left.a = _q3dMatrixTemp[3][0] + _q3dMatrixTemp[0][0];
	left.b = _q3dMatrixTemp[3][1] + _q3dMatrixTemp[0][1];
	left.c = _q3dMatrixTemp[3][2] + _q3dMatrixTemp[0][2];
	left.d = _q3dMatrixTemp[3][3] + _q3dMatrixTemp[0][3];
*/
	right.a = -1; //_q3dMatrixTemp[3][0] - _q3dMatrixTemp[0][0];
	right.b = 0; //_q3dMatrixTemp[3][1] - _q3dMatrixTemp[0][1];
	right.c = 0; //_q3dMatrixTemp[3][2] - _q3dMatrixTemp[0][2];
	right.d = 0; //_q3dMatrixTemp[3][3] - _q3dMatrixTemp[0][3];

	near.a = 0; //_q3dMatrixTemp[2][0];
	near.b = 0; //_q3dMatrixTemp[2][1];
	near.c = -1; //_q3dMatrixTemp[2][2];
	near.d = 0.150; //_q3dMatrixTemp[2][3];

	far.a = 0;
	far.b = 0;
	far.c = 1;
	far.d = -0.1;

/*
	left.a *= NUM10;
	right.a *= NUM10;
	left.b *= 240;
	right.b *= 240;
*/

	MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)

		if (st->ltrig && st->rtrig) {
			vid_screen_shot("/pc/tmp/screen.png");
		}
/*
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
*/
	MAPLE_FOREACH_END()


	
	// draw
	// todo...
	pvr_prim(&fillerLevel.defaultHeader, sizeof(pvr_poly_hdr_t));

	pvr_dr_state_t state;
	pvr_dr_init(state);
	pvr_vertex_t *vert;
	q3dTypeVector v;
	int pos = 0;
	for (int z = 0; z < NUM1; z++) { 
		for (int x = 0; x < NUM1; x++, pos++) {
//			if (finalPositions[pos].z < 0) {
//				continue;
//			}
			// todo: cull invisible stuff
			int v1 = z * NUM + x;
			int v2 = v1 + NUM;
			int v3 = v1 + 1;
			int v4 = v2 + 1;

			int color = 0xff00;

			int c = z * NUM1 + x;
			v.x = positionWorld[c].x;
			v.y = positionWorld[c].y;
			v.z = positionWorld[c].z;
			float l = q3dPlanePointInPlane(&left, &v);
			float r = q3dPlanePointInPlane(&right, &v);
			float n = q3dPlanePointInPlane(&near, &v);
			float f = q3dPlanePointInPlane(&far, &v);

			if (v.z > 0.125) color = 0xff0000;

//			if (l < -1.75 || r < -1.75 || n < -0.25/* || f < -1*/) {
				// definately outside
//				color = 0xff0000;
//				continue;
//			} else if (l < -0.75 || r < -0.75 || n < 0/* || f < 0*/) {
				// some parts inside
//				color = 0xffff00;
//				continue;
//			} else {
				// fully inside
//				color = 0xff00;
//			}

			if (vertexScreen[v1].z < 0 ||
				vertexScreen[v2].z < 0 ||
				vertexScreen[v3].z < 0 ||
				vertexScreen[v4].z < 0
//				finalVertex[v1].z < 0.5 ||
//				finalVertex[v2].z < 0.5 ||
//				finalVertex[v3].z < 0.5 ||
//				finalVertex[v4].z < 0.5
				) {
					continue;
			}

			vert = pvr_dr_target(state);
			vert->x = vertexScreen[v1].x;
			vert->y = vertexScreen[v1].y;
			vert->z = vertexScreen[v1].z;
			vert->argb = color;
			vert->flags = PVR_CMD_VERTEX;
			pvr_dr_commit(vert);

			vert = pvr_dr_target(state);
			vert->x = vertexScreen[v2].x;
			vert->y = vertexScreen[v2].y;
			vert->z = vertexScreen[v2].z;
			vert->argb = color;
			vert->flags = PVR_CMD_VERTEX;
			pvr_dr_commit(vert);

			vert = pvr_dr_target(state);
			vert->x = vertexScreen[v3].x;
			vert->y = vertexScreen[v3].y;
			vert->z = vertexScreen[v3].z;
			vert->argb = color;
			vert->flags = PVR_CMD_VERTEX;
			pvr_dr_commit(vert);

			vert = pvr_dr_target(state);
			vert->x = vertexScreen[v4].x;
			vert->y = vertexScreen[v4].y;
			vert->z = vertexScreen[v4].z;
			vert->argb = color;
			vert->flags = PVR_CMD_VERTEX_EOL;
			pvr_dr_commit(vert);
		}
	}

}

