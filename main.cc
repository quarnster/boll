#include <kos.h>
#include <math.h>
#include <png/png.h>

#include <q3d.h>

// #include "player.h"
#include "game.h"

q3dTypePolyhedron *generatePlane(float size) {
	q3dTypePolyhedron *plane = (q3dTypePolyhedron*) malloc(sizeof(q3dTypePolyhedron));
	q3dPolyhedronInit(plane);

	int NUM = 5;
	float fNUM = ((float) NUM / 2.0f);
	plane->vertexLength = NUM * NUM;
	plane->vertex = (q3dTypeVertex*) malloc(plane->vertexLength * sizeof(q3dTypeVertex));

	int pos = 0;
	for (int z = 0; z < NUM; z++) {
		float zPos = (z / fNUM) * size - fNUM * size;
		for (int x = 0; x < NUM; x++) {
			float xPos = (x / fNUM) * size - fNUM * size;
			q3dVertexSet3f(&plane->vertex[pos], xPos, 0.0f, zPos);
			pos++;
		}
	}

	plane->polygonLength = 1;
	plane->polygon = (q3dTypePolygon*) malloc(1 * sizeof(q3dTypePolyhedron));

	// front
	q3dPolygonInit(&plane->polygon[0]);
	plane->polygon[0].vertexLength = (NUM - 1) * NUM * 2;
	plane->polygon[0].vertex = (uint16*) malloc(plane->polygon[0].vertexLength * sizeof(uint16));

	pos = 0;
	int pos2 = 0;
	for (int i = 0; i < NUM-1; i++) {
		for (int j = 0; j < NUM; j++) {
			printf("%d, %d     ", pos, pos + NUM);
			plane->polygon[0].vertex[pos2] = pos;
			pos2++;
			plane->polygon[0].vertex[pos2] = pos + NUM;
			pos2++;
			pos++;

		}
		printf("\n");
	}



	return plane;
}

q3dTypePolyhedron * generateTest(float max) {
	q3dTypePolyhedron *cube = (q3dTypePolyhedron*) malloc(sizeof(q3dTypePolyhedron));
	q3dPolyhedronInit(cube);

	int NUM = 10;
	cube->vertexLength = NUM * 2;
	cube->vertex = (q3dTypeVertex*) malloc(cube->vertexLength * sizeof(q3dTypeVertex));

	int pos = 0;
	for (int i = 0; i < NUM; i++) {
		float z = (i / (float) (NUM / 2.0f)) * max - max;
		q3dVertexSet3f(&cube->vertex[pos], -20.0f,  0.0f,  z);
		pos++;
		q3dVertexSet3f(&cube->vertex[pos],  20.0f,  0.0f,  z);
		pos++;
	}

	cube->polygonLength = 1;
	cube->polygon = (q3dTypePolygon*) malloc(1 * sizeof(q3dTypePolyhedron));

	q3dPolygonInit(&cube->polygon[0]);
	cube->polygon[0].vertexLength = NUM*2;
	cube->polygon[0].vertex = (uint16*) malloc(cube->polygon[0].vertexLength * sizeof(uint16));
	for (int i = 0; i < cube->polygon[0].vertexLength; i++) {
		cube->polygon[0].vertex[i] = i;
	}

	return cube;
}

uint32 qtime = 0;
void handle_time(irq_t source, irq_context_t *context) {
	int blah = timer_clear(TMU0);
	if (!blah) qtime+=2;
}

extern uint8 romdisk[];

KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);
KOS_INIT_ROMDISK(romdisk);
/*
void init_pvr() {
	pvr_init_params_t params = {
		// Enable opaque and translucent polygons with size 16 
		{ PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_0 },
		
		// Vertex buffer size 512K 
		512*1024
	};      

	pvr_init(&params);
}
*/
pvr_poly_cxt_t loadImage(char *name) {
	pvr_poly_cxt_t	cxt;
	pvr_ptr_t	ptr;
	uint32		width;
	uint32		height;

	int ret = png_load_texture(name, &ptr, PNG_NO_ALPHA, &width, &height);

	if (ret) {
		printf("*****************************\n\nerror loading image!!!!\n\n*****************************\n\n");
	}

	pvr_poly_cxt_txr(
		&cxt,
		PVR_LIST_OP_POLY,
		PVR_TXRFMT_RGB565 | PVR_TXRFMT_TWIDDLED,
		width,
		height,
		ptr,
		PVR_FILTER_BILINEAR
	);

	return cxt;
}
extern "C" int snd_init();

bool done = false;
void ccallback(uint8 addr, uint32 buttons) {
	done = true;
}
int main(int argc, char **argv) {
	// Initialize KOS
	pvr_init_defaults();
	vid_set_mode(DM_640x480, PM_RGB565);

	pvr_set_bg_color(1.0f, 1.0f, 1.0f);

	// Get basic stuff initialized
	timer_prime(TMU0, 500, 1);
	timer_start(TMU0);

	irq_set_handler(TIMER_IRQ, &handle_time);

//	snd_init();
//	sfxhnd_t bounce = snd_sfx_load("/pc/tmp/bounce.wav");
//	snd_sfx_play(bounce, 15, 15);
	qtime = 0;

	Game game;
	cont_btn_callback(0, CONT_START | CONT_A | CONT_B | CONT_X | CONT_Y, (cont_btn_callback_t) ccallback);
	while(!done) {
		game.update();
		game.draw();
	}
        pvr_stats_t stat;
        pvr_get_stats(&stat);
        printf("framerate1: %f\n", stat.frame_rate);

	pvr_mem_reset();
	timer_stop(TMU0);

	return 0;
}
