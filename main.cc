#include <kos.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#include <q3d.h>
#include <arch/rtc.h>

// #include "player.h"
#include "disclaimer.h"
#include "mainmenu.h"
#include "ntscmenu.h"
#include "game.h"
#include "credits.h"

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

uint32 gametime = 0;
void handle_time(irq_t source, irq_context_t *context) {
	int blah = timer_clear(TMU0);
	if (!blah) gametime+=2;
}

KOS_INIT_FLAGS(INIT_DEFAULT /*INIT_IRQ*/ | INIT_MALLOCSTATS);

pvr_init_params_t pvr_params = {
	{ PVR_BINSIZE_32, PVR_BINSIZE_0, PVR_BINSIZE_8, PVR_BINSIZE_0, PVR_BINSIZE_16 },
	1 * 4*512 * 1024
};

extern "C" int snd_init();

bool done2 = false;
void ccallback(uint8 addr, uint32 buttons) {
	done2 = true;
}


enum {
	GAME = 0,
	LEVEL,
	BALL,
	CREDITS,
	MAINMENU,
	NTSCMENU
};

int main(int argc, char **argv) {
	// Initialize KOS
	pvr_init(&pvr_params);
	srand(rtc_unix_secs());

#ifndef FINAL
	fs_chdir("/pc/home/quarn/code/dreamcast/game/data");
#else
	fs_chdir("/cd/data");
#endif

	loadFont();

#ifndef	BETA
	Disclaimer d;
	// Select the correct video-mode
	if (vid_check_cable() == CT_VGA) {
		vid_set_mode(DM_640x480_VGA, PM_RGB565);
	} else if (flashrom_get_region() == FLASHROM_REGION_EUROPE) {
		vid_set_mode(DM_640x480_PAL_IL,PM_RGB565);
		NtscMenu m;
		m.showMenu();
	} else {
		vid_set_mode(DM_640x480_NTSC_IL, PM_RGB565);
	}
	d.show();
#else
	vid_set_mode(DM_640x480, PM_RGB565);
#endif

	// Get basic stuff initialized
//	timer_prime(TMU1, 500, 1);
//	timer_start(TMU1);

//	irq_set_handler(TIMER_IRQ, &handle_time);

	uint64 t1 = timer_ms_gettime64();

	snd_init();

	gametime = 0;



#ifndef FINAL
	cont_btn_callback(0, CONT_START | CONT_A | CONT_B | CONT_X | CONT_Y, (cont_btn_callback_t) ccallback);
#endif

	pvr_fog_table_color(1, 0, 0, 0);
	pvr_fog_table_exp2(0.010f);

	spu_cdda_volume(255, 255);
	loadResources();

	Game game;
	Credits credits;
	MainMenu mmenu;


	while ((timer_ms_gettime64() - t1) < 20000);

	int mode = MAINMENU;

	while (!done2) {
//		gametime = timer_ms_gettime64();
		switch (mode) {
			case MAINMENU:
				mode = mmenu.showMenu();
				break;
			case GAME:
				game.run();
				mode = MAINMENU;
				break;
			case CREDITS:
				credits.run();
				mode = MAINMENU;
				break;
			default:
				mode = mmenu.showMenu();
				break;
		}
	}

	cdrom_cdda_pause();

	freeResources();
        pvr_stats_t stat;
	printf("get stats..\n");
        pvr_get_stats(&stat);
        printf("framerate1: %f\n", stat.frame_rate);

	pvr_mem_reset();

	return 0;
}
