#include <math.h>
#include "level.h"

extern q3dTypeFiller fillerLevel;

#define NUM 33
#define NUM1 32
static q3dTypeVertex vertex[3*NUM*NUM] __attribute__((aligned(32)));
static q3dTypeVertex vertexScreen[3*NUM*NUM] __attribute__((aligned(32)));

static q3dTypeVertex sky[4] __attribute__((aligned(32)));
static q3dTypeVertex skyScreen[4] __attribute__((aligned(32)));

static q3dTypeVertex position[NUM1*NUM1] __attribute__((aligned(32)));
static q3dTypeVertex positionWorld[NUM1*NUM1] __attribute__((aligned(32)));

static q3dTypeVector normals[5] __attribute__((aligned(32)));
static q3dTypeVector normalsWorld[5] __attribute__((aligned(32)));

uint8 levelData[NUM1 * NUM1];


static q3dTypeLevel data;

Level::Level() {
	float size = LEVELSIZE;
	float size2 = LEVELHEIGHT;

	int pos = 0;
	// generate level
	data.vertexLength = NUM * NUM;
	data.polygonLength = 0;
	for (int z = 0; z < NUM1; z++) {
		for (int x = 0; x < NUM1; x++) {
			if (x % 4 == 0&& z % 4 == 0) {
				levelData[pos] = HOLE;
				data.polygonLength += 5;
				data.vertexLength += 4;
			} else if (x % 7 == 1 && z % 8 == 1) {
				levelData[pos] = HIGH;
				data.polygonLength += 4;
				data.vertexLength += 4;
			} else {
				levelData[pos] = NORMAL;
				data.polygonLength++;
			}
			data.vertexLength += 4;
			pos++;
		}
	}

	// baselevel vertex-data
	data.vertex = (q3dTypeVertex*) malloc(data.vertexLength * sizeof(q3dTypeVertex));
	data.polygon = (q3dTypePolygon*) malloc(data.polygonLength * sizeof(q3dTypePolygon));

	for (int i = 0; i < data.polygonLength; i++) {
		q3dPolygonInit(&data.polygon[i]);
	}
	pos = 0;
	int polypos = 0;
	int vertpos = 0;
	float frac = ((1 / (float) NUM1) * 2 - 1) * size;
	for (int z = 0; z < NUM1; z++) {
		float zpos = (z / (float) NUM1) * 2 - 1;
		zpos *= size;

		for (int x = 0; x < NUM1; x++) {
			float xpos = (x / (float) NUM1) * 2 - 1;
			xpos *= size;

			int lp = z * NUM1 + x; // position in leveldata

			q3dVertexSet3f(&data.vertex[vertpos], xpos, 0, zpos); vertpos++;
			q3dVertexSet3f(&data.vertex[vertpos], xpos, 0, zpos-frac); vertpos++;
			q3dVertexSet3f(&data.vertex[vertpos], xpos+frac, 0, zpos); vertpos++;
			q3dVertexSet3f(&data.vertex[vertpos], xpos+frac, 0, zpos-frac); vertpos++;

			if (levelData[lp] == NORMAL) {
//				q3dPolygonInit(&data.polygon[polypos]);
				data.polygon[polypos].vertexLength = 4;
				data.polygon[polypos].vertex = (uint16*) malloc(4 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 4;
				data.polygon[polypos].vertex[1] = vertpos - 3;
				data.polygon[polypos].vertex[2] = vertpos - 2;
				data.polygon[polypos].vertex[3] = vertpos - 1;
				polypos++;
/*
			} else if (levelData[pos] == HOLE) {
				q3dVertexSet3f(&data.vertex[vertpos], xpos, -LEVELHEIGHT, zpos); vertpos++;
				q3dVertexSet3f(&data.vertex[vertpos], xpos, -LEVELHEIGHT, zpos-frac); vertpos++;
				q3dVertexSet3f(&data.vertex[vertpos], xpos+frac, -LEVELHEIGHT, zpos); vertpos++;
				q3dVertexSet3f(&data.vertex[vertpos], xpos+frac, -LEVELHEIGHT, zpos-frac); vertpos++;


				// top polygon
				q3dPolygonInit(&data.polygon[polypos]);
				data.polygon[polypos].vertexLength = 4;
				data.polygon[polypos].vertex = (uint16*) malloc(4 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 4;
				data.polygon[polypos].vertex[1] = vertpos - 3;
				data.polygon[polypos].vertex[2] = vertpos - 2;
				data.polygon[polypos].vertex[3] = vertpos - 1;
				polypos++;

				// front
				q3dPolygonInit(&data.polygon[polypos]);
				data.polygon[polypos].vertexLength = 4;
				datapolygon[polypos].vertex = (uint16*) malloc(4 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 4-4;
				data.polygon[polypos].vertex[1] = vertpos - 2-4;
				data.polygon[polypos].vertex[2] = vertpos - 4;
				data.polygon[polypos].vertex[3] = vertpos - 2;
				polypos++;
*/
			}
		}
	}
	if (polypos != data.polygonLength) {
		printf("polypos != data.polygonLength!! (%d, %d)", polypos, data.polygonLength);
	}
	if (vertpos != data.vertexLength) {
		printf("vertpos != data.vertexLength!! (%d, %d)", vertpos, data.vertexLength);
	}

/*
	pos = 0;
	for (int z = 0; z < NUM; z++) {
		float zpos = (z / (float) NUM1) * 2 - 1;
		zpos *= size;
		for (int x = 0; x < NUM; x++) {
			float xpos = (x / (float) NUM1) * 2 - 1;
			xpos *= size;
			vertex[pos].x = xpos;
			vertex[pos].y = 0;
			vertex[pos].z = -zpos;
			pos++;
		}
	}
*/
	pos = 0;
	for (int z = 0; z < NUM1; z++) {
//		float zpos = (vertex[z * NUM].z + vertex[z * NUM + NUM].z) / 2.0f;;
		for (int x = 0; x < NUM1; x++) {
			int p2 = z * NUM + x;
			float xpos = (vertex[p2].x + vertex[p2+1].x) / 2.0f;;
			float zpos = (vertex[p2].z + vertex[p2 + NUM].z)/ 2.0f;
			q3dVertexSet3f(&position[pos], xpos, 0, zpos);
			position[pos].w = 1;
			pos++;
		}
	}

	q3dVectorSet3f(&normals[0],  0, -1,  0);
	q3dVectorSet3f(&normals[1], -1,  0,  0);
	q3dVectorSet3f(&normals[2],  0,  0, -1);
	q3dVectorSet3f(&normals[3],  1,  0,  0);
	q3dVectorSet3f(&normals[4],  0,  0,  1);


	q3dVertexSet3f(&sky[0], -size, -20, size);
	q3dVertexSet3f(&sky[1], -size, -20, -size);
	q3dVertexSet3f(&sky[2],  size, -20, size);
	q3dVertexSet3f(&sky[3],  size, -20, -size);
}

/*
Level::Level() {
	float size = LEVELSIZE;
	float size2 = LEVELHEIGHT;

	int pos = 0;
 	for (int i = 0, y = 2; i < 3; i++, y -= size2) {
 		if (i == 1) y = 0;
 		for (int z = 0; z < NUM; z++) {
 			float zpos = (z / (float) NUM1) * 2 - 1;
 			zpos *= size;
 			for (int x = 0; x < NUM; x++) {
 				float xpos = (x / (float) NUM1) * 2 - 1;
 				xpos *= size;
 				vertex[pos].x = xpos;
 				vertex[pos].y = y;
 				vertex[pos].z = -zpos;
 				pos++;
			}
		}
	}

	pos = 0;
	for (int z = 0; z < NUM1; z++) {
//		float zpos = (vertex[z * NUM].z + vertex[z * NUM + NUM].z) / 2.0f;;
		for (int x = 0; x < NUM1; x++) {
			int p2 = z * NUM + x;
			float xpos = (vertex[p2].x + vertex[p2+1].x) / 2.0f;;
			float zpos = (vertex[p2].z + vertex[p2 + NUM].z)/ 2.0f;
			q3dVertexSet3f(&position[pos], xpos, 0, zpos);
			position[pos].w = 1;
			pos++;
		}
	}

	// generate level
	pos = 0;
	for (int z = 0; z < NUM1; z++) {
		for (int x = 0; x < NUM1; x++) {
			if (x % 4 == 0&& z % 4 == 0) {
				levelData[pos] = HOLE;
			} else if (x % 7 == 1 && z % 8 == 1) {
				levelData[pos] = HIGH;
			} else {
				levelData[pos] = NORMAL;
			}
			pos++;
		}
	}

	q3dVectorSet3f(&normals[0],  0, -1,  0);
	q3dVectorSet3f(&normals[1], -1,  0,  0);
	q3dVectorSet3f(&normals[2],  0,  0, -1);
	q3dVectorSet3f(&normals[3],  1,  0,  0);
	q3dVectorSet3f(&normals[4],  0,  0,  1);


	q3dVertexSet3f(&sky[0], -size, -20, size);
	q3dVertexSet3f(&sky[1], -size, -20, -size);
	q3dVertexSet3f(&sky[2],  size, -20, size);
	q3dVertexSet3f(&sky[3],  size, -20, -size);
}
*/
Level::~Level() {
}

void Level::update() {
}

/*
#define q3dPlanePointInPlane(plane, point) { \
	register float __a __asm__("fr0") = (((q3dTypePlane*) plane)->a); \
	register float __b __asm__("fr1") = (((q3dTypePlane*) plane)->b); \
	register float __c __asm__("fr2") = (((q3dTypePlane*) plane)->c); \
	register float __x __asm__("fr4") = (((q3dTypeVector*) point)->x); \
	register float __y __asm__("fr5") = (((q3dTypeVector*) point)->y); \
	register float __z __asm__("fr6") = (((q3dTypeVector*) point)->z); \
	__asm__ __volatile__( \
		"fldi0	fr3\n" \
		"fldi0	fr7\n" \
		"fipr	fv0,fv1\n" \
		"fmov	fr4,fr0\n" \
		: "=f" (__a), "=f" (__b), "=f" (__c) \
		: "0" (__a), "1" (__b), "2" (__c), "4" (__x), "5" (__y), "6" (__z) \
		: "fr3", "fr7"); \
}
*/
/*
inline float q3dPlanePointInPlane(q3dTypePlane *plane, q3dTypeVector *point) {
	float q = 0;
	register float __x1 __asm__("fr0") = plane->a;
	register float __y1 __asm__("fr1") = plane->b;
	register float __z1 __asm__("fr2") = plane->c;
	register float __w1 __asm__("fr3") = 0;
	register float __x2 __asm__("fr4") = point->x;
	register float __y2 __asm__("fr5") = point->y;
	register float __z2 __asm__("fr6") = point->z;
	register float __w2 __asm__("fr7") = 0;
	__asm__ __volatile__(
		"fipr	fv0,fv4\n"
		"fmov	fr7,fr0\n"
		: "=f" (__x1), "=f" (__y1), "=f" (__z1), "=f" (__w1), "=f" (__x2), "=f" (__y2), "=f" (__z2), "=f" (__w2)
		: "0" (__x1), "1" (__y1), "2" (__z1), "3" (__w1), "4" (__x2), "5" (__y2), "6" (__z2), "7" (__w2)
		);

	q = __x1;
	//	cross(plane->a, plane->b, plane->c, point->x, point->y, point->z);
	return q + plane->d;
}
*/
//extern float q3dPlanePointInPlane(q3dTypePlane *plane, q3dTypeVector *point);
/*
#define mat_trans_single(x, y, z) { \
	register float __x __asm__("fr0") = (x); \
	register float __y __asm__("fr1") = (y); \
	register float __z __asm__("fr2") = (z); \
	__asm__ __volatile__( \
		"fldi1	fr3\n" \
		"ftrv	xmtrx,fv0\n" \
		"fldi1	fr2\n" \
		"fdiv	fr3,fr2\n" \
		"fmul	fr2,fr0\n" \
		"fmul	fr2,fr1\n" \
		: "=f" (__x), "=f" (__y), "=f" (__z) \
		: "0" (__x), "1" (__y), "2" (__z) \
		: "fr3" ); \
	x = __x; y = __y; z = __z; \
}
*/
extern matrix_t projection_matrix;
extern matrix_t screen_matrix;
bool print = false;
bool pr = false;
bool press = false;

uint32 polysent;
uint32 vertextest;

void Level::draw() {



	q3dMatrixIdentity();

//	q3dMatrixLoad(&projection_matrix);
	q3dMatrixApply(&_q3dMatrixCamera);
	q3dMatrixTransform(position, positionWorld, NUM1*NUM1, sizeof(q3dTypeVertex));

//	q3dMatrixRotateY(
//	q3dMatrixStore(&_q3dMatrixTemp);
	q3dMatrixTransformNormals(normals, normalsWorld, 5);

	// transform vertices
//	q3dMatrixLoad(&_q3dMatrixPerspective);
	q3dMatrixLoad(&screen_matrix);
	q3dMatrixApply(&projection_matrix);
	q3dMatrixApply(&_q3dMatrixCamera); 

	mat_transform(vertex, vertexScreen, 3*NUM*NUM, sizeof(q3dTypeVertex));
//	q3dMatrixLoad(&_q3dMatrixPer
//	q3dMatrixLoad(&_q3dMatrixCamera);
//	q3dMatrixTransform(positions, &finalPositions[0].x, NUM1*NUM1, sizeof(q3dTypeVertex));

	q3dTypePlane left;
	q3dTypePlane right;
	q3dTypePlane near;
	q3dTypePlane far;
	q3dTypePlane top;
	q3dTypePlane bottom;

	q3dMatrixIdentity();
//	q3dMatrixApply(&_q3dMatrixCamera);
	q3dMatrixApply(&projection_matrix);
	q3dMatrixStore(&_q3dMatrixTemp);

	left.a = _q3dMatrixTemp[0][3] + _q3dMatrixTemp[0][0];
	left.b = _q3dMatrixTemp[1][3] + _q3dMatrixTemp[1][0];
	left.c = _q3dMatrixTemp[2][3] + _q3dMatrixTemp[2][0];
	left.d = _q3dMatrixTemp[3][3] + _q3dMatrixTemp[3][0];

	right.a = _q3dMatrixTemp[0][3] - _q3dMatrixTemp[0][0];
	right.b = _q3dMatrixTemp[1][3] - _q3dMatrixTemp[1][0];
	right.c = _q3dMatrixTemp[2][3] - _q3dMatrixTemp[2][0];
	right.d = _q3dMatrixTemp[3][3] - _q3dMatrixTemp[3][0];

	near.a = _q3dMatrixTemp[0][2];
	near.b = _q3dMatrixTemp[1][2];
	near.c = _q3dMatrixTemp[2][2];
	near.d = _q3dMatrixTemp[3][2];

	far.a = 0;
	far.b = 0;
	far.c = 1;
	far.d = -0.1;

	q3dPlaneNormalize(&left);
	q3dPlaneNormalize(&right);
	q3dPlaneNormalize(&near);
	q3dPlaneNormalize(&far);

	int32 color[5];
	for (int i = 0; i < 5; i++) {
		float shade = 0.5 + normalsWorld[i].z * 0.5f;
		shade = shade < 0 ? 0 : shade > 1 ? 1 : shade;

		color[i] = PVR_PACK_COLOR(1, shade, shade, shade);
//		color[i] = 0xffffffff;
	}

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

			printf("near.a: %f\n", near.a);
			printf("near.b: %f\n", near.b);
			printf("near.c: %f\n", near.c);
			printf("near.d: %f\n", near.d);

			printf("far.a: %f\n", far.a);
			printf("far.b: %f\n", far.b);
			printf("far.c: %f\n", far.c);
			printf("far.d: %f\n", far.d);

			printf("matrix:\n");
			printf("\t%f\t%f\t%f\t%f\n", _q3dMatrixTemp[0][0], _q3dMatrixTemp[0][1], _q3dMatrixTemp[0][2], _q3dMatrixTemp[0][3]);
			printf("\t%f\t%f\t%f\t%f\n", _q3dMatrixTemp[1][0], _q3dMatrixTemp[1][1], _q3dMatrixTemp[1][2], _q3dMatrixTemp[1][3]);
			printf("\t%f\t%f\t%f\t%f\n", _q3dMatrixTemp[2][0], _q3dMatrixTemp[2][1], _q3dMatrixTemp[2][2], _q3dMatrixTemp[2][3]);
			printf("\t%f\t%f\t%f\t%f\n", _q3dMatrixTemp[3][0], _q3dMatrixTemp[3][1], _q3dMatrixTemp[3][2], _q3dMatrixTemp[3][3]);
		} else if (!(st->buttons & CONT_Y)) {
			print = false;
		}
/*
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

	int add = NUM * NUM;
	for (int z = 0; z < NUM1; z++) { 
		for (int x = 0; x < NUM1; x++, pos++) {
//			if (levelData[pos] == HOLE)
//				continue;

			// todo: cull invisible stuff
			int v1 = add + z * NUM + x;
			int v2 = v1 + NUM;
			int v3 = v1 + 1;
			int v4 = v2 + 1;

//			int color = 0xff00;


			int c = z * NUM1 + x;
			v.x = positionWorld[c].x;
			v.y = positionWorld[c].y;
			v.z = positionWorld[c].z;
			float l = q3dPlanePointInPlane(&left, &v);
			float r = q3dPlanePointInPlane(&right, &v);
			float n = q3dPlanePointInPlane(&near, &v);
			float f = q3dPlanePointInPlane(&far, &v);

			int fcol = 0xff00;
//			if (v.z > 0.125) color = 0xff0000;

			vertextest += 6;
			if (l < -10 || r < -10 || n < -10) {
				// definately outside
				vertextest -= 3;
				fcol = 0xff0000;
				continue;
			} else if (fabs(l) < 10 || fabs(r) < 10 || fabs(n) < 10) {
				// some parts inside
				fcol = 0xffff00;
				// do some clipping..
				continue;
			} else {
				// fully inside
				fcol = 0xff00;
			}

			for (int xx = 0; xx < 5; xx++) {
				color[xx] = fcol;
			}
/*
			vertextest += 4;

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
*/
			if (levelData[pos] == HIGH) {
				// todo: cull invisible stuff
				int v5 = v1 + add;
				int v6 = v2 + add;
				int v7 = v3 + add;
				int v8 = v4 + add;
/*
				vertextest += 4;

				if (vertexScreen[v5].z < 0 ||
					vertexScreen[v6].z < 0 ||
					vertexScreen[v7].z < 0 ||
					vertexScreen[v8].z < 0
				) {
					continue;
				}
*/
				polysent += 5 * 2; // count triangles
				// left
				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v1].x;
				vert->y = vertexScreen[v1].y;
				vert->z = vertexScreen[v1].z;
				vert->u = 0; vert->v = 1;
				vert->argb = color[1];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v2].x;
				vert->y = vertexScreen[v2].y;
				vert->z = vertexScreen[v2].z;
				vert->u = 0; vert->v = 0;
				vert->argb = color[1];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v5].x;
				vert->y = vertexScreen[v5].y;
				vert->z = vertexScreen[v5].z;
				vert->u = 1; vert->v = 1;
				vert->argb = color[1];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v6].x;
				vert->y = vertexScreen[v6].y;
				vert->z = vertexScreen[v6].z;
				vert->u = 1; vert->v = 0;
				vert->argb = color[1];
				vert->flags = PVR_CMD_VERTEX_EOL;
				pvr_dr_commit(vert);

				// front
				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v2].x;
				vert->y = vertexScreen[v2].y;
				vert->z = vertexScreen[v2].z;
				vert->u = 0; vert->v = 1;
				vert->argb = color[2];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v4].x;
				vert->y = vertexScreen[v4].y;
				vert->z = vertexScreen[v4].z;
				vert->u = 0; vert->v = 0;
				vert->argb = color[2];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v6].x;
				vert->y = vertexScreen[v6].y;
				vert->z = vertexScreen[v6].z;
				vert->u = 1; vert->v = 1;
				vert->argb = color[2];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v8].x;
				vert->y = vertexScreen[v8].y;
				vert->z = vertexScreen[v8].z;
				vert->u = 1; vert->v = 0;
				vert->argb = color[2];
				vert->flags = PVR_CMD_VERTEX_EOL;
				pvr_dr_commit(vert);

				// right
				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v4].x;
				vert->y = vertexScreen[v4].y;
				vert->z = vertexScreen[v4].z;
				vert->u = 0; vert->v = 1;
				vert->argb = color[3];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v3].x;
				vert->y = vertexScreen[v3].y;
				vert->z = vertexScreen[v3].z;
				vert->u = 0; vert->v = 0;
				vert->argb = color[3];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v8].x;
				vert->y = vertexScreen[v8].y;
				vert->z = vertexScreen[v8].z;
				vert->u = 1; vert->v = 1;
				vert->argb = color[3];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v7].x;
				vert->y = vertexScreen[v7].y;
				vert->z = vertexScreen[v7].z;
				vert->u = 1; vert->v = 0;
				vert->argb = color[3];
				vert->flags = PVR_CMD_VERTEX_EOL;
				pvr_dr_commit(vert);

				// back
				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v3].x;
				vert->y = vertexScreen[v3].y;
				vert->z = vertexScreen[v3].z;
				vert->u = 0; vert->v = 1;
				vert->argb = color[4];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v1].x;
				vert->y = vertexScreen[v1].y;
				vert->z = vertexScreen[v1].z;
				vert->u = 0; vert->v = 0;
				vert->argb = color[4];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v7].x;
				vert->y = vertexScreen[v7].y;
				vert->z = vertexScreen[v7].z;
				vert->u = 1; vert->v = 1;
				vert->argb = color[4];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v5].x;
				vert->y = vertexScreen[v5].y;
				vert->z = vertexScreen[v5].z;
				vert->u = 1; vert->v = 0;
				vert->argb = color[4];
				vert->flags = PVR_CMD_VERTEX_EOL;
				pvr_dr_commit(vert);

				// top
				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v5].x;
				vert->y = vertexScreen[v5].y;
				vert->z = vertexScreen[v5].z;
				vert->u = 0; vert->v = 1;
				vert->argb = color[0];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v6].x;
				vert->y = vertexScreen[v6].y;
				vert->z = vertexScreen[v6].z;
				vert->u = 0; vert->v = 0;
				vert->argb = color[0];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v7].x;
				vert->y = vertexScreen[v7].y;
				vert->z = vertexScreen[v7].z;
				vert->u = 1; vert->v = 1;
				vert->argb = color[0];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v8].x;
				vert->y = vertexScreen[v8].y;
				vert->z = vertexScreen[v8].z;
				vert->u = 1; vert->v = 0;
				vert->argb = color[0];
				vert->flags = PVR_CMD_VERTEX_EOL;
				pvr_dr_commit(vert);
			} else if (levelData[pos] == HOLE) {
				// todo: cull invisible stuff
				int v5 = v1 - add;
				int v6 = v2 - add;
				int v7 = v3 - add;
				int v8 = v4 - add;
/*
				vertextest += 4;
				if (vertexScreen[v5].z < 0 ||
					vertexScreen[v6].z < 0 ||
					vertexScreen[v7].z < 0 ||
					vertexScreen[v8].z < 0
				) {
					continue;
				}
*/
				polysent += 4 * 2;
				// left
				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v1].x;
				vert->y = vertexScreen[v1].y;
				vert->z = vertexScreen[v1].z;
				vert->u = 0; vert->v = 1;
				vert->argb = color[3];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v2].x;
				vert->y = vertexScreen[v2].y;
				vert->z = vertexScreen[v2].z;
				vert->u = 0; vert->v = 0;
				vert->argb = color[3];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v5].x;
				vert->y = vertexScreen[v5].y;
				vert->z = vertexScreen[v5].z;
				vert->u = 1; vert->v = 1;
				vert->argb = color[3];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v6].x;
				vert->y = vertexScreen[v6].y;
				vert->z = vertexScreen[v6].z;
				vert->u = 1; vert->v = 0;
				vert->argb = color[3];
				vert->flags = PVR_CMD_VERTEX_EOL;
				pvr_dr_commit(vert);

				// front
				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v2].x;
				vert->y = vertexScreen[v2].y;
				vert->z = vertexScreen[v2].z;
				vert->u = 0; vert->v = 1;
				vert->argb = color[4];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v4].x;
				vert->y = vertexScreen[v4].y;
				vert->z = vertexScreen[v4].z;
				vert->u = 0; vert->v = 0;
				vert->argb = color[4];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v6].x;
				vert->y = vertexScreen[v6].y;
				vert->z = vertexScreen[v6].z;
				vert->u = 1; vert->v = 1;
				vert->argb = color[4];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v8].x;
				vert->y = vertexScreen[v8].y;
				vert->z = vertexScreen[v8].z;
				vert->u = 1; vert->v = 0;
				vert->argb = color[4];
				vert->flags = PVR_CMD_VERTEX_EOL;
				pvr_dr_commit(vert);

				// right
				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v4].x;
				vert->y = vertexScreen[v4].y;
				vert->z = vertexScreen[v4].z;
				vert->u = 0; vert->v = 1;
				vert->argb = color[1];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v3].x;
				vert->y = vertexScreen[v3].y;
				vert->z = vertexScreen[v3].z;
				vert->u = 0; vert->v = 0;
				vert->argb = color[1];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v8].x;
				vert->y = vertexScreen[v8].y;
				vert->z = vertexScreen[v8].z;
				vert->u = 1; vert->v = 1;
				vert->argb = color[1];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v7].x;
				vert->y = vertexScreen[v7].y;
				vert->z = vertexScreen[v7].z;
				vert->u = 1; vert->v = 0;
				vert->argb = color[1];
				vert->flags = PVR_CMD_VERTEX_EOL;
				pvr_dr_commit(vert);

				// back
				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v3].x;
				vert->y = vertexScreen[v3].y;
				vert->z = vertexScreen[v3].z;
				vert->u = 0; vert->v = 1;
				vert->argb = color[2];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v1].x;
				vert->y = vertexScreen[v1].y;
				vert->z = vertexScreen[v1].z;
				vert->u = 0; vert->v = 0;
				vert->argb = color[2];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v7].x;
				vert->y = vertexScreen[v7].y;
				vert->z = vertexScreen[v7].z;
				vert->u = 1; vert->v = 1;
				vert->argb = color[2];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v5].x;
				vert->y = vertexScreen[v5].y;
				vert->z = vertexScreen[v5].z;
				vert->u = 1; vert->v = 0;
				vert->argb = color[2];
				vert->flags = PVR_CMD_VERTEX_EOL;
				pvr_dr_commit(vert);
/*
				// top
				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v5].x;
				vert->y = vertexScreen[v5].y;
				vert->z = vertexScreen[v5].z;
				vert->u = 0; vert->v = 1;
				vert->argb = color[0];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v6].x;
				vert->y = vertexScreen[v6].y;
				vert->z = vertexScreen[v6].z;
				vert->u = 0; vert->v = 0;
				vert->argb = color[0];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v7].x;
				vert->y = vertexScreen[v7].y;
				vert->z = vertexScreen[v7].z;
				vert->u = 1; vert->v = 1;
				vert->argb = color[0];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v8].x;
				vert->y = vertexScreen[v8].y;
				vert->z = vertexScreen[v8].z;
				vert->u = 1; vert->v = 0;
				vert->argb = color[0];
				vert->flags = PVR_CMD_VERTEX_EOL;
				pvr_dr_commit(vert);
*/
			} else {
				polysent += 1 * 2;
				// draw normal (flat)
				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v1].x;
				vert->y = vertexScreen[v1].y;
				vert->z = vertexScreen[v1].z;
				vert->u = 0; vert->v = 1;
				vert->argb = color[0];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v2].x;
				vert->y = vertexScreen[v2].y;
				vert->z = vertexScreen[v2].z;
				vert->u = 0; vert->v = 0;
				vert->argb = color[0];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v3].x;
				vert->y = vertexScreen[v3].y;
				vert->z = vertexScreen[v3].z;
				vert->u = 1; vert->v = 1;
				vert->argb = color[0];
				vert->flags = PVR_CMD_VERTEX;
				pvr_dr_commit(vert);

				vert = pvr_dr_target(state);
				vert->x = vertexScreen[v4].x;
				vert->y = vertexScreen[v4].y;
				vert->z = vertexScreen[v4].z;
				vert->u = 1; vert->v = 0;
				vert->argb = color[0];
				vert->flags = PVR_CMD_VERTEX_EOL;
				pvr_dr_commit(vert);
			}
		}
	}

}

