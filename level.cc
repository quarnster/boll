#include <math.h>
#include "level.h"

extern q3dTypeFiller fillerLevel;

#define NUM 33
#define NUM1 32
/*
static q3dTypeVertex vertex[3*NUM*NUM] __attribute__((aligned(32)));
static q3dTypeVertex vertexScreen[3*NUM*NUM] __attribute__((aligned(32)));

static q3dTypeVertex sky[4] __attribute__((aligned(32)));
static q3dTypeVertex skyScreen[4] __attribute__((aligned(32)));
*/
static q3dTypeVertex position[NUM1*NUM1] __attribute__((aligned(32)));
static q3dTypeVertex positionWorld[NUM1*NUM1] __attribute__((aligned(32)));

static q3dTypeVector normals[5] __attribute__((aligned(32)));
static q3dTypeVector normalsWorld[5] __attribute__((aligned(32)));

uint8 levelData[NUM1 * NUM1];


static q3dTypeLevel data;

#define setTex1(polygon) \
			polygon.texel = (q3dTypeTexel*) malloc(3 * sizeof(q3dTypeTexel)); \
			polygon.texel[0].u = 0; \
			polygon.texel[0].v = 0; \
			polygon.texel[1].u = 0; \
			polygon.texel[1].v = 1; \
			polygon.texel[2].u = 1; \
			polygon.texel[2].v = 0;

#define setTex2(polygon) \
			polygon.texel = (q3dTypeTexel*) malloc(3 * sizeof(q3dTypeTexel)); \
			polygon.texel[0].u = 1; \
			polygon.texel[0].v = 1; \
			polygon.texel[1].u = 1; \
			polygon.texel[1].v = 0; \
			polygon.texel[2].u = 0; \
			polygon.texel[2].v = 1;

#define setTexQuad(polygon) \
			polygon.texel = (q3dTypeTexel*) malloc(4 * sizeof(q3dTypeTexel)); \
			polygon.texel[0].u = 0; \
			polygon.texel[0].v = 0; \
			polygon.texel[1].u = 0; \
			polygon.texel[1].v = 1; \
			polygon.texel[2].u = 1; \
			polygon.texel[2].v = 0; \
			polygon.texel[3].u = 1; \
			polygon.texel[3].v = 1; \


Level::Level() {
	float size = LEVELSIZE;
	float size2 = LEVELHEIGHT;

	int pos = 0;
	// generate level
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
	pos = 0;

	// generate level from data
	data.vertexLength = 0/*NUM * NUM*/;
	data.polygonLength = 0;

	for (int z = 0; z < NUM1; z++) {
		for (int x = 0; x < NUM1; x++) {
			if (levelData[pos] == HOLE) {
//				data.polygonLength += 4;
//				data.vertexLength += 4;
			} else if (levelData[pos] == HIGH) {
				data.polygonLength += 5;
//				data.polygonLength += 5;
				data.vertexLength += 4;
				data.vertexLength += 4;
			} else {
				data.vertexLength += 4;
				data.polygonLength++;
			}
//			data.vertexLength += 4;
			pos++;
		}
	}

	data.polygonLength *= 2; // quads to triangles

	// baselevel vertex-data
	data.vertex = (q3dTypeVertex*) malloc(data.vertexLength * sizeof(q3dTypeVertex) + 32);
	data.polygon = (q3dTypePolygon*) malloc(data.polygonLength * sizeof(q3dTypePolygon));
	/* align to 32 bytes */
//	data.vertex = (q3dTypeVertex*) (((uint32)data.vertex & ~31) + 32);

	for (int i = 0; i < data.polygonLength; i++) {
		q3dPolygonInit(&data.polygon[i]);
	}
	pos = 0;
	int polypos = 0;
	int vertpos = 0;
	float frac = ((1 / (float) NUM1) * 2) * size;
	for (int z = 0; z < NUM1; z++) {
		float zpos = (z / (float) NUM1) * 2 - 1;
		zpos *= size;

		for (int x = 0; x < NUM1; x++,pos++) {
			float xpos = (x / (float) NUM1) * 2 - 1;
			xpos *= size;

			int lp = pos; // position in leveldata

			if (levelData[lp] != HOLE) {
				q3dVertexSet3f(&data.vertex[vertpos], xpos, 0, zpos); vertpos++;
				q3dVertexSet3f(&data.vertex[vertpos], xpos, 0, zpos+frac); vertpos++;
				q3dVertexSet3f(&data.vertex[vertpos], xpos+frac, 0, zpos); vertpos++;
				q3dVertexSet3f(&data.vertex[vertpos], xpos+frac, 0, zpos+frac); vertpos++;
			}

			if (levelData[lp] == NORMAL) {

				data.polygon[polypos].vertexLength = 3;
				data.polygon[polypos].vertex = (uint16*) malloc(3 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 4;
				data.polygon[polypos].vertex[1] = vertpos - 3;
				data.polygon[polypos].vertex[2] = vertpos - 2;
				setTex1(data.polygon[polypos]);
				polypos++;

				data.polygon[polypos].vertexLength = 3;
				data.polygon[polypos].vertex = (uint16*) malloc(3 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 1;
				data.polygon[polypos].vertex[1] = vertpos - 2;
				data.polygon[polypos].vertex[2] = vertpos - 3;
				setTex2(data.polygon[polypos]);
				polypos++;
/*
				data.polygon[polypos].vertexLength = 4;
				data.polygon[polypos].vertex = (uint16*) malloc(4 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 4;
				data.polygon[polypos].vertex[1] = vertpos - 3;
				data.polygon[polypos].vertex[2] = vertpos - 2;
				data.polygon[polypos].vertex[3] = vertpos - 1;
				setTexQuad(data.polygon[polypos]);
				polypos++;
*/
			} else if (levelData[lp] == HIGH) {
				q3dVertexSet3f(&data.vertex[vertpos], xpos, LEVELHEIGHT, zpos); vertpos++;
				q3dVertexSet3f(&data.vertex[vertpos], xpos, LEVELHEIGHT, zpos+frac); vertpos++;
				q3dVertexSet3f(&data.vertex[vertpos], xpos+frac, LEVELHEIGHT, zpos); vertpos++;
				q3dVertexSet3f(&data.vertex[vertpos], xpos+frac, LEVELHEIGHT, zpos+frac); vertpos++;
/*
				data.polygon[polypos].vertexLength = 4;
				data.polygon[polypos].vertex = (uint16*) malloc(4 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 4;
				data.polygon[polypos].vertex[1] = vertpos - 3;
				data.polygon[polypos].vertex[2] = vertpos - 2;
				data.polygon[polypos].vertex[3] = vertpos - 1;
				setTexQuad(data.polygon[polypos]);
				polypos++;

				// front
				data.polygon[polypos].vertexLength = 4;
				data.polygon[polypos].vertex = (uint16*) malloc(4 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 4-4;
				data.polygon[polypos].vertex[1] = vertpos - 4;
				data.polygon[polypos].vertex[2] = vertpos - 2-4;
				data.polygon[polypos].vertex[3] = vertpos - 2;
				setTexQuad(data.polygon[polypos]);
				polypos++;

				// left
				data.polygon[polypos].vertexLength = 4;
				data.polygon[polypos].vertex = (uint16*) malloc(4 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 4-4;
				data.polygon[polypos].vertex[1] = vertpos - 3-4;
				data.polygon[polypos].vertex[2] = vertpos - 4;
				data.polygon[polypos].vertex[3] = vertpos - 3;
				setTexQuad(data.polygon[polypos]);
				polypos++;

				// right
				data.polygon[polypos].vertexLength = 4;
				data.polygon[polypos].vertex = (uint16*) malloc(4 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 1-4;
				data.polygon[polypos].vertex[1] = vertpos - 2-4;
				data.polygon[polypos].vertex[2] = vertpos - 1;
				data.polygon[polypos].vertex[3] = vertpos - 2;
				setTexQuad(data.polygon[polypos]);
				polypos++;
*/

				// top polygon
				data.polygon[polypos].vertexLength = 3;
				data.polygon[polypos].vertex = (uint16*) malloc(3 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 4;
				data.polygon[polypos].vertex[1] = vertpos - 3;
				data.polygon[polypos].vertex[2] = vertpos - 2;
				setTex1(data.polygon[polypos]);
				polypos++;

				data.polygon[polypos].vertexLength = 3;
				data.polygon[polypos].vertex = (uint16*) malloc(3 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 1;
				data.polygon[polypos].vertex[1] = vertpos - 2;
				data.polygon[polypos].vertex[2] = vertpos - 3;
				setTex2(data.polygon[polypos]);
				polypos++;


				// front
				data.polygon[polypos].vertexLength = 3;
				data.polygon[polypos].vertex = (uint16*) malloc(3 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 4-4;
				data.polygon[polypos].vertex[1] = vertpos - 4;
				data.polygon[polypos].vertex[2] = vertpos - 2-4;
				setTex1(data.polygon[polypos]);
				polypos++;

				data.polygon[polypos].vertexLength = 3;
				data.polygon[polypos].vertex = (uint16*) malloc(3 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 2;
				data.polygon[polypos].vertex[1] = vertpos - 2-4;
				data.polygon[polypos].vertex[2] = vertpos - 4;
				setTex2(data.polygon[polypos]);
				polypos++;

				// back
				data.polygon[polypos].vertexLength = 3;
				data.polygon[polypos].vertex = (uint16*) malloc(3 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 3-4;
				data.polygon[polypos].vertex[1] = vertpos - 1-4;
				data.polygon[polypos].vertex[2] = vertpos - 3;
				setTex1(data.polygon[polypos]);
				polypos++;

				data.polygon[polypos].vertexLength = 3;
				data.polygon[polypos].vertex = (uint16*) malloc(3 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 1;
				data.polygon[polypos].vertex[1] = vertpos - 3;
				data.polygon[polypos].vertex[2] = vertpos - 1-4;
				setTex2(data.polygon[polypos]);
				polypos++;

				// left
				data.polygon[polypos].vertexLength = 3;
				data.polygon[polypos].vertex = (uint16*) malloc(3 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 4-4;
				data.polygon[polypos].vertex[1] = vertpos - 3-4;
				data.polygon[polypos].vertex[2] = vertpos - 4;
				setTex1(data.polygon[polypos]);
				polypos++;

				data.polygon[polypos].vertexLength = 3;
				data.polygon[polypos].vertex = (uint16*) malloc(3 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 3;
				data.polygon[polypos].vertex[1] = vertpos - 4;
				data.polygon[polypos].vertex[2] = vertpos - 3-4;
				setTex2(data.polygon[polypos]);
				polypos++;

				// right
				data.polygon[polypos].vertexLength = 3;
				data.polygon[polypos].vertex = (uint16*) malloc(3 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 1-4;
				data.polygon[polypos].vertex[1] = vertpos - 2-4;
				data.polygon[polypos].vertex[2] = vertpos - 1;
				setTex1(data.polygon[polypos]);
				polypos++;

				data.polygon[polypos].vertexLength = 3;
				data.polygon[polypos].vertex = (uint16*) malloc(3 * sizeof(uint16));

				data.polygon[polypos].vertex[0] = vertpos - 2;
				data.polygon[polypos].vertex[1] = vertpos - 1;
				data.polygon[polypos].vertex[2] = vertpos - 2-4;
				setTex2(data.polygon[polypos]);
				polypos++;

			}
		}
	}
	if (polypos != data.polygonLength) {
		printf("polypos != data.polygonLength!! (%d, %d)", polypos, data.polygonLength);
	}
	if (vertpos != data.vertexLength) {
		printf("vertpos != data.vertexLength!! (%d, %d)", vertpos, data.vertexLength);
	}
	printf("polys: %d\nvertices: %d\n", data.polygonLength, data.vertexLength);

	printf("optimizing:\n");

	uint16 *idx = new uint16[data.vertexLength];
	uint16 *na = new uint16[data.vertexLength];
	int sub = 0;
	pos = 0;

	for (int i = 0; i < data.vertexLength; i++) {
		bool unique = true;
		for (int j = 0; j < pos; j++) {
			if (data.vertex[i].x == data.vertex[na[j]].x &&
				data.vertex[i].y == data.vertex[na[j]].y &&
				data.vertex[i].z == data.vertex[na[j]].z)
			{
				sub++;
				idx[i] = j;
				unique = false;
				break;
			}
		}
		if (unique) {
			idx[i] = pos;
			na[pos] = i;
			pos++;
		}
	}

	printf("%d vertices was the same... vertex count: %d, ratio: %d\% better. (pos: %d)\n", sub, data.vertexLength-sub, (int) (100.0f * (data.vertexLength / ((float) (data.vertexLength-sub)))), pos);

	q3dTypeVertex *tmpVertex = (q3dTypeVertex*) malloc(pos * sizeof(q3dTypeVertex) + 32);
	tmpVertex = (q3dTypeVertex*) (((uint32)tmpVertex & ~31) + 32);

	for (int i = 0; i < pos; i++) {
		q3dVertexSetV(&tmpVertex[i], &data.vertex[na[i]]);
	}
	free(data.vertex);
	data.vertex = tmpVertex;
	data.vertexLength = pos;

	for (int i = 0; i < data.polygonLength; i++) {
		for (int j = 0; j < data.polygon[i].vertexLength; j++) {
			data.polygon[i].vertex[j] = idx[data.polygon[i].vertex[j]];
		}
	}
	/* dummy...*/
	malloc(16384+64);
	vertexScreen = (q3dTypeVertex*) malloc(data.vertexLength * sizeof(q3dTypeVertex) + 32);
	vertexCam = (q3dTypeVertex*) malloc(data.vertexLength * sizeof(q3dTypeVertex) + 32);
	vertexScreen = (q3dTypeVertex*) (((uint32)vertexScreen & ~31) + 32);
	vertexCam = (q3dTypeVertex*) (((uint32)vertexCam & ~31) + 32);

	delete[] idx;
	delete[] na;
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
/*
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
*/

	q3dVectorSet3f(&normals[0],  0, -1,  0);
	q3dVectorSet3f(&normals[1], -1,  0,  0);
	q3dVectorSet3f(&normals[2],  0,  0, -1);
	q3dVectorSet3f(&normals[3],  1,  0,  0);
	q3dVectorSet3f(&normals[4],  0,  0,  1);

/*
	q3dVertexSet3f(&sky[0], -size, -20, size);
	q3dVertexSet3f(&sky[1], -size, -20, -size);
	q3dVertexSet3f(&sky[2],  size, -20, size);
	q3dVertexSet3f(&sky[3],  size, -20, -size);
*/
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
//	free(vertexScreen);
//	free(vertexCam);
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

inline float fmax(float f1, float f2) {
	return f1 < f2 ? f2 : f1;
}
inline void clip(pvr_dr_state_t state, q3dTypePolygon *poly, q3dTypeVertex *vertexCam, q3dTypeVertex *vertexScreen) {

	q3dTypeVertex temp;
	float near_clip_z = 0.001f;
	float near_clip_z_w = 0.001f;
	int stat = 0;
	int inside = 0;

	for (int i = 0; i < 3; i++) {
		if (vertexCam[poly->vertex[i]].z < near_clip_z) {
			stat |= (1 << (i));
		} else inside++;
	}
	if (inside == 0) {
		// polygon completly outside
		return;
	}
	polysent++;
	int color = 0xffffff;

	int i0;
	int i1;
	int i2;

	// texture indices
	int tx0;
	int tx1;
	int tx2;

	pvr_vertex_t *vert;
/*
	if (inside == 1) {
		if (!(stat & 1)) {
			tx0 = 0;
			tx1 = 1;
			tx2 = 2;
			i0 = poly->vertex[0];
			i1 = poly->vertex[1];
			i2 = poly->vertex[2];
		} else if (!(stat & 2)) {
			tx0 = 1;
			tx1 = 2;
			tx2 = 0;
			i0 = poly->vertex[1];
			i1 = poly->vertex[2];
			i2 = poly->vertex[0];
		} else {
			tx0 = 2;
			tx1 = 0;
			tx2 = 1;
			i0 = poly->vertex[2];
			i1 = poly->vertex[0];
			i2 = poly->vertex[1];
		}
		float t1 = (near_clip_z - vertexCam[i0].z) / (vertexCam[i1].z - vertexCam[i0].z);
		float t2 = (near_clip_z - vertexCam[i0].z) / (vertexCam[i2].z - vertexCam[i0].z);

//		temp.x = w0->x;
//		temp.y = w0->y;
//		temp.z = w0->z;
//		mat_trans_single(temp.x, temp.y, temp.z);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->x = vertexScreen[i0].x;
		vert->y = vertexScreen[i0].y;
		vert->z = vertexScreen[i0].z;
		vert->u = poly->texel[tx0].u;
		vert->v = poly->texel[tx0].v;
		vert->argb = color;
		pvr_dr_commit(vert);

		temp.x = vertexCam[i0].x + t1 * (vertexCam[i1].x - vertexCam[i0].x);
		temp.y = vertexCam[i0].y + t1 * (vertexCam[i1].y - vertexCam[i0].y);
		temp.z = near_clip_z;
		mat_trans_single(temp.x, temp.y, temp.z);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->x = temp.x;
		vert->y = temp.y;
		vert->z = temp.z;
		vert->u = poly->texel[tx0].u + t1 * (poly->texel[tx1].u - poly->texel[tx0].u);
		vert->v = poly->texel[tx0].v + t1 * (poly->texel[tx1].v - poly->texel[tx0].v);
		vert->argb = color;
		pvr_dr_commit(vert);

		temp.x = vertexCam[i0].x + t2 * (vertexCam[i2].x - vertexCam[i0].x);
		temp.y = vertexCam[i0].y + t2 * (vertexCam[i2].y - vertexCam[i0].y);
		temp.z = near_clip_z;
//		temp.z = w0->z + t1 * (w2->z - w0->z);
		mat_trans_single(temp.x, temp.y, temp.z);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX_EOL;
		vert->x = temp.x;
		vert->y = temp.y;
		vert->z = temp.z;
		vert->u = poly->texel[tx0].u + t2 * (poly->texel[tx2].u - poly->texel[tx0].u);
		vert->v = poly->texel[tx0].v + t2 * (poly->texel[tx2].v - poly->texel[tx0].v);
		vert->argb = color;
		pvr_dr_commit(vert);
	} else if (inside == 2) {

		// two points inside
		if (stat & 1) {
			tx0 = 0;
			tx1 = 1;
			tx2 = 2;
			i0 = poly->vertex[0];
			i1 = poly->vertex[1];
			i2 = poly->vertex[2];
		} else if (stat & 2) {
			tx0 = 1;
			tx1 = 2;
			tx2 = 0;
			i0 = poly->vertex[1];
			i1 = poly->vertex[2];
			i2 = poly->vertex[0];
		} else {
			tx0 = 2;
			tx1 = 0;
			tx2 = 1;
			i0 = poly->vertex[2];
			i1 = poly->vertex[0];
			i2 = poly->vertex[1];
		}
		float t1 = (near_clip_z - vertexCam[i0].z) / (vertexCam[i1].z - vertexCam[i0].z);
		float t2 = (near_clip_z - vertexCam[i0].z) / (vertexCam[i2].z - vertexCam[i0].z);

		temp.x = vertexCam[i0].x + t1 * (vertexCam[i1].x - vertexCam[i0].x);
		temp.y = vertexCam[i0].y + t1 * (vertexCam[i1].y - vertexCam[i0].y);
		temp.z = near_clip_z;
		mat_trans_single(temp.x, temp.y, temp.z);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->x = temp.x;
		vert->y = temp.y;
		vert->z = temp.z;
		vert->u = poly->texel[tx0].u + t1 * (poly->texel[tx1].u - poly->texel[tx0].u);
		vert->v = poly->texel[tx0].v + t1 * (poly->texel[tx1].v - poly->texel[tx0].v);
		vert->argb = color;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->x = vertexScreen[i1].x;
		vert->y = vertexScreen[i1].y;
		vert->z = vertexScreen[i1].z;
		vert->u = poly->texel[tx1].u;
		vert->v = poly->texel[tx1].v;
		vert->argb = color;
		pvr_dr_commit(vert);

		temp.x = vertexCam[i0].x + t2 * (vertexCam[i2].x - vertexCam[i0].x);
		temp.y = vertexCam[i0].y + t2 * (vertexCam[i2].y - vertexCam[i0].y);
		temp.z = near_clip_z;
		mat_trans_single(temp.x, temp.y, temp.z);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->x = temp.x;
		vert->y = temp.y;
		vert->z = temp.z;
		vert->u = poly->texel[tx0].u + t2 * (poly->texel[tx2].u - poly->texel[tx0].u);
		vert->v = poly->texel[tx0].v + t2 * (poly->texel[tx2].v - poly->texel[tx0].v);
		vert->argb = color;
		pvr_dr_commit(vert);


		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX_EOL;
		vert->x = vertexScreen[i2].x;
		vert->y = vertexScreen[i2].y;
		vert->z = vertexScreen[i2].z;
		vert->u = poly->texel[tx2].u;
		vert->v = poly->texel[tx2].v;
		vert->argb = color;
		pvr_dr_commit(vert);
	} else {
*/
	if (inside == 3) {
		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->x = vertexScreen[poly->vertex[0]].x;
		vert->y = vertexScreen[poly->vertex[0]].y;
		vert->z = vertexScreen[poly->vertex[0]].z;
		vert->u = poly->texel[0].u;
		vert->v = poly->texel[0].v;
		vert->argb = color;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->x = vertexScreen[poly->vertex[1]].x;
		vert->y = vertexScreen[poly->vertex[1]].y;
		vert->z = vertexScreen[poly->vertex[1]].z;
		vert->u = poly->texel[1].u;
		vert->v = poly->texel[1].v;
		vert->argb = color;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX_EOL;
		vert->x = vertexScreen[poly->vertex[2]].x;
		vert->y = vertexScreen[poly->vertex[2]].y;
		vert->z = vertexScreen[poly->vertex[2]].z;
		vert->u = poly->texel[2].u;
		vert->v = poly->texel[2].v;
		vert->argb = color;
		pvr_dr_commit(vert);
	}
}

void Level::draw() {



//	q3dMatrixLoad(&projection_matrix);
	q3dMatrixLoad(&_q3dMatrixCamera);
//	q3dMatrixTransform(position, positionWorld, NUM1*NUM1, sizeof(q3dTypeVertex));
//	mat_transform(data.vertex, vertexCam, data.vertexLength, sizeof(q3dTypeVertex));
	q3dMatrixTransform(data.vertex, vertexCam, data.vertexLength, sizeof(q3dTypeVertex));

//	q3dMatrixRotateY(
//	q3dMatrixStore(&_q3dMatrixTemp);
//	q3dMatrixTransformNormals(normals, normalsWorld, 5);

	// transform vertices
//	q3dMatrixLoad(&_q3dMatrixPerspective);
	q3dMatrixLoad(&screen_matrix);
	q3dMatrixApply(&projection_matrix);
	q3dMatrixApply(&_q3dMatrixCamera);

	mat_transform(data.vertex, vertexScreen, data.vertexLength, sizeof(q3dTypeVertex));
//	q3dMatrixTransformPVR(data.vertex, vertexScreen, data.vertexLength, sizeof(q3dTypeVertex));

//	mat_transform(vertex, vertexScreen, 3*NUM*NUM, sizeof(q3dTypeVertex));
	
//	q3dMatrixLoad(&_q3dMatrixPer
//	q3dMatrixLoad(&_q3dMatrixCamera);
//	q3dMatrixTransform(positions, &finalPositions[0].x, NUM1*NUM1, sizeof(q3dTypeVertex));
/*
	q3dTypePlane left;
	q3dTypePlane right;
	q3dTypePlane near;
	q3dTypePlane far;
	q3dTypePlane top;
	q3dTypePlane bottom;

	q3dMatrixLoad(&projection_matrix);
	q3dMatrixStore(&_q3dMatrixTemp);

	_q3dMatrixTemp[3][2] = - _q3dMatrixTemp[3][2];

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
*/
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
/*
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

	MAPLE_FOREACH_END()
*/

	// draw
	// todo...

	// for the clipper..
	q3dMatrixLoad(&screen_matrix);
	q3dMatrixApply(&projection_matrix);
//	q3dMatrixApply(&_q3dMatrixCamera);
	pvr_prim(&fillerLevel.defaultHeader, sizeof(pvr_poly_hdr_t));

	pvr_vertex_t *vert;
	q3dTypeVertex *vert2;
	pvr_dr_state_t state;
	pvr_dr_init(state);

	for (int i = 0; i < data.polygonLength; i++) {
		int j;
		int test = 0;
		bool cont = false;
		float far_clip_z = 100.0f;

		int len = data.polygon[i].vertexLength;
		int color = 0xffffff;
		q3dTypePolygon * poly = &data.polygon[i];

//		for (j = 0; j < len; j++) {
//			if (q3dPlanePointInPlane(&right, &vertexCam[data.polygon[i].vertex[j]]) < -10) {
//				test++;
//			}
//		}

		// test if polygon is outside the right plane
		if (vertexCam[poly->vertex[0]].x > vertexCam[poly->vertex[0]].z+1)
			test++;
		if (vertexCam[poly->vertex[1]].x > vertexCam[poly->vertex[1]].z+1)
			test++;
		if (vertexCam[poly->vertex[2]].x > vertexCam[poly->vertex[2]].z+1)
			test++;
		if (test == 3)
			continue;

		test = 0;
		// test if polygon is outside the left plane
		if (vertexCam[poly->vertex[0]].x < -vertexCam[poly->vertex[0]].z-1)
			test++;
		if (vertexCam[poly->vertex[1]].x < -vertexCam[poly->vertex[1]].z-1)
			test++;
		if (vertexCam[poly->vertex[2]].x < -vertexCam[poly->vertex[2]].z-1)
			test++;
		if (test == 3)
			continue;

		// test if polygon is outside the far plane
		if (vertexCam[poly->vertex[0]].z > far_clip_z)
			test++;
		if (vertexCam[poly->vertex[1]].z > far_clip_z)
			test++;
		if (vertexCam[poly->vertex[2]].z > far_clip_z)
			test++;
		if (test == 3)
			continue;

		q3dTypeVertex temp;
		float near_clip_z = 1.0f;
		float near_clip_z_w = 0.001f;
		int stat = 0;
		int inside = 0;

		if (vertexCam[poly->vertex[0]].z < near_clip_z) {
			stat |= 1;
		} else inside++;

		if (vertexCam[poly->vertex[1]].z < near_clip_z) {
			stat |= 2;
		} else inside++;

		if (vertexCam[poly->vertex[2]].z < near_clip_z) {
			stat |= 4;
		} else inside++;

		if (inside == 0) {
			// polygon completly outside
			continue;
		}
		polysent++;

		int i0;
		int i1;
		int i2;

		// texture indices
		int tx0;
		int tx1;
		int tx2;

		pvr_vertex_t *vert;
		q3dTypeVertex *vert2;

		if (inside == 1) {
			if (!(stat & 1)) {
				tx0 = 0;
				tx1 = 1;
				tx2 = 2;
				i0 = poly->vertex[0];
				i1 = poly->vertex[1];
				i2 = poly->vertex[2];
			} else if (!(stat & 2)) {
				tx0 = 1;
				tx1 = 2;
				tx2 = 0;
				i0 = poly->vertex[1];
				i1 = poly->vertex[2];
				i2 = poly->vertex[0];
			} else {
				tx0 = 2;
				tx1 = 0;
				tx2 = 1;
				i0 = poly->vertex[2];
				i1 = poly->vertex[0];
				i2 = poly->vertex[1];
			}
			float t1 = (near_clip_z - vertexCam[i0].z) / (vertexCam[i1].z - vertexCam[i0].z);
			float t2 = (near_clip_z - vertexCam[i0].z) / (vertexCam[i2].z - vertexCam[i0].z);

	//		temp.x = w0->x;
	//		temp.y = w0->y;
	//		temp.z = w0->z;
	//		mat_trans_single(temp.x, temp.y, temp.z);

			vert = pvr_dr_target(state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = vertexScreen[i0].x;
			vert->y = vertexScreen[i0].y;
			vert->z = vertexScreen[i0].z;
			vert->u = poly->texel[tx0].u;
			vert->v = poly->texel[tx0].v;
			vert->argb = color;
			pvr_dr_commit(vert);

			temp.x = vertexCam[i0].x + t1 * (vertexCam[i1].x - vertexCam[i0].x);
			temp.y = vertexCam[i0].y + t1 * (vertexCam[i1].y - vertexCam[i0].y);
			temp.z = near_clip_z;
			mat_trans_single(temp.x, temp.y, temp.z);

			vert = pvr_dr_target(state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = temp.x;
			vert->y = temp.y;
			vert->z = temp.z;
			vert->u = poly->texel[tx0].u + t1 * (poly->texel[tx1].u - poly->texel[tx0].u);
			vert->v = poly->texel[tx0].v + t1 * (poly->texel[tx1].v - poly->texel[tx0].v);
			vert->argb = color;
			pvr_dr_commit(vert);

			temp.x = vertexCam[i0].x + t2 * (vertexCam[i2].x - vertexCam[i0].x);
			temp.y = vertexCam[i0].y + t2 * (vertexCam[i2].y - vertexCam[i0].y);
			temp.z = near_clip_z;
	//		temp.z = w0->z + t1 * (w2->z - w0->z);
			mat_trans_single(temp.x, temp.y, temp.z);

			vert = pvr_dr_target(state);
			vert->flags = PVR_CMD_VERTEX_EOL;
			vert->x = temp.x;
			vert->y = temp.y;
			vert->z = temp.z;
			vert->u = poly->texel[tx0].u + t2 * (poly->texel[tx2].u - poly->texel[tx0].u);
			vert->v = poly->texel[tx0].v + t2 * (poly->texel[tx2].v - poly->texel[tx0].v);
			vert->argb = color;
			pvr_dr_commit(vert);
		} else if (inside == 2) {
			// two points inside
			if (stat & 1) {
				tx0 = 0;
				tx1 = 1;
				tx2 = 2;
				i0 = poly->vertex[0];
				i1 = poly->vertex[1];
				i2 = poly->vertex[2];
			} else if (stat & 2) {
				tx0 = 1;
				tx1 = 2;
				tx2 = 0;
				i0 = poly->vertex[1];
				i1 = poly->vertex[2];
				i2 = poly->vertex[0];
			} else {
				tx0 = 2;
				tx1 = 0;
				tx2 = 1;
				i0 = poly->vertex[2];
				i1 = poly->vertex[0];
				i2 = poly->vertex[1];
			}
			float t1 = (near_clip_z - vertexCam[i0].z) / (vertexCam[i1].z - vertexCam[i0].z);
			float t2 = (near_clip_z - vertexCam[i0].z) / (vertexCam[i2].z - vertexCam[i0].z);

			temp.x = vertexCam[i0].x + t1 * (vertexCam[i1].x - vertexCam[i0].x);
			temp.y = vertexCam[i0].y + t1 * (vertexCam[i1].y - vertexCam[i0].y);
			temp.z = near_clip_z;
			mat_trans_single(temp.x, temp.y, temp.z);

			vert = pvr_dr_target(state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = temp.x;
			vert->y = temp.y;
			vert->z = temp.z;
			vert->u = poly->texel[tx0].u + t1 * (poly->texel[tx1].u - poly->texel[tx0].u);
			vert->v = poly->texel[tx0].v + t1 * (poly->texel[tx1].v - poly->texel[tx0].v);
			vert->argb = color;
			pvr_dr_commit(vert);

			vert = pvr_dr_target(state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = vertexScreen[i1].x;
			vert->y = vertexScreen[i1].y;
			vert->z = vertexScreen[i1].z;
			vert->u = poly->texel[tx1].u;
			vert->v = poly->texel[tx1].v;
			vert->argb = color;
			pvr_dr_commit(vert);

			temp.x = vertexCam[i0].x + t2 * (vertexCam[i2].x - vertexCam[i0].x);
			temp.y = vertexCam[i0].y + t2 * (vertexCam[i2].y - vertexCam[i0].y);
			temp.z = near_clip_z;
			mat_trans_single(temp.x, temp.y, temp.z);

			vert = pvr_dr_target(state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = temp.x;
			vert->y = temp.y;
			vert->z = temp.z;
			vert->u = poly->texel[tx0].u + t2 * (poly->texel[tx2].u - poly->texel[tx0].u);
			vert->v = poly->texel[tx0].v + t2 * (poly->texel[tx2].v - poly->texel[tx0].v);
			vert->argb = color;
			pvr_dr_commit(vert);

			vert = pvr_dr_target(state);
			vert->flags = PVR_CMD_VERTEX_EOL;
			vert->x = vertexScreen[i2].x;
			vert->y = vertexScreen[i2].y;
			vert->z = vertexScreen[i2].z;
			vert->u = poly->texel[tx2].u;
			vert->v = poly->texel[tx2].v;
			vert->argb = color;
			pvr_dr_commit(vert);
		} else if (inside == 3) {
			vert2 = &vertexScreen[poly->vertex[0]];
			vert = pvr_dr_target(state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = vert2->x;
			vert->y = vert2->y;
			vert->z = vert2->z;
			vert->u = poly->texel[0].u;
			vert->v = poly->texel[0].v;
			vert->argb = color;
			pvr_dr_commit(vert);

			vert2 = &vertexScreen[poly->vertex[1]];
			vert = pvr_dr_target(state);
			vert->flags = PVR_CMD_VERTEX;
			vert->x = vert2->x;
			vert->y = vert2->y;
			vert->z = vert2->z;
			vert->u = poly->texel[1].u;
			vert->v = poly->texel[1].v;
			vert->argb = color;
			pvr_dr_commit(vert);

			vert2 = &vertexScreen[poly->vertex[2]];
			vert = pvr_dr_target(state);
			vert->flags = PVR_CMD_VERTEX_EOL;
			vert->x = vert2->x;
			vert->y = vert2->y;
			vert->z = vert2->z;
			vert->u = poly->texel[2].u;
			vert->v = poly->texel[2].v;
			vert->argb = color;
			pvr_dr_commit(vert);
		}
	}
}

