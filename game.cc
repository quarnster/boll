#include <string.h>
#include "game.h"
#include "lib/primitives.h"

#include <plx/font.h>
#include <plx/list.h>
#include <plx/dr.h>

static point_t w;
extern uint32 polysent;
extern uint32 vertextest;

#define GAMELENGTH 0.25

q3dTypePolyhedron *sphere;
q3dTypePolyhedron *scorePolyhedron;
q3dTypeFiller fillerPlayers;
q3dTypeFiller fillerLevel;
q3dTypeFiller fillerScore;

q3dTypeMatrix screen_matrix __attribute__((aligned(32))) = {
    { 640/4.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 480 / 4.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

Game::Game() {
	gameended = false;

	q3dMatrixInit();

	sphere = generateSphere(6, 6);
	scorePolyhedron = generateSphere(3, 3);

	q3dFillerCellInit(&fillerPlayers);
	fillerPlayers.defaultCxt.gen.clip_mode = PVR_USERCLIP_INSIDE;
	fillerPlayers.defaultCxt.gen.fog_type = PVR_FOG_TABLE;
	pvr_poly_compile(&fillerPlayers.defaultHeader, &fillerPlayers.defaultCxt);

	q3dFillerTextureInit(&fillerLevel);
	fillerLevel.defaultCxt = loadImage("buzz.png", PVR_LIST_OP_POLY);
	fillerLevel.defaultCxt.gen.clip_mode = PVR_USERCLIP_INSIDE;
	fillerLevel.defaultCxt.gen.fog_type = PVR_FOG_TABLE;
	pvr_poly_compile(&fillerLevel.defaultHeader, &fillerLevel.defaultCxt);

	q3dFillerStandardInit(&fillerScore);
	fillerScore.defaultCxt.gen.clip_mode = PVR_USERCLIP_INSIDE;
//	fillerScore.defaultCxt.gen.fog_type = PVR_FOG_TABLE;
	pvr_poly_compile(&fillerScore.defaultHeader, &fillerScore.defaultCxt);

	q3dCameraInit(&cam);
	cam._pos.y = 5;
	cam._pos.z = -5;

	player = new Player[4];
	for (int i = 0; i < 4; i++)
		player[i].setController(i);

	player[0].position.set( 0,  0,  +2);
	player[1].position.set( 0,  10, +2);
	player[2].position.set(-2,  2, -2);
	player[3].position.set(+2,  2, -2);

	pvr_poly_cxt_t cxt;
	pvr_poly_cxt_col(
		&cxt,
		PVR_LIST_OP_POLY
	);

	pvr_poly_compile(&crossHeader, &cxt);

	pvr_poly_cxt_col(
		&cxt,
		PVR_LIST_TR_POLY
	);

	pvr_poly_compile(&fadeHeader, &cxt);

	w.x = 20;
	w.y = 32;
	w.z = 100;
}

Game::~Game() {
	printf("free sphere: ...");
	q3dPolyhedronFree(sphere);
	printf("done!\nfree sphere2: ...");
	free(sphere);
	printf("done!\ndelete[] player: ...");
	delete[] player;
	printf("done!\n");
}

extern uint32 gametime;
void Game::reset() {
	done = false;
	gamestart = timer_ms_gettime64();
	gametime = 0;
	gameended = false;

	for (int i = 0; i < 4; i++)
		player[i].score = 0;

	player[0].position.set( 0,  0,  +2);
	player[1].position.set( 0,  10, +2);
	player[2].position.set(-2,  2, -2);
	player[3].position.set(+2,  2, -2);

}

//extern bool done;
void Game::run() {
	reset();
	while (!done /*&& (gametime < 60 * 1000 * GAMELENGTH)*/) {
		// TODO: check for exit.. ?
		gametime = timer_ms_gettime64() - gamestart;
		if (gametime >= 60 * 1000 * GAMELENGTH) {
			gameended = true;
		}

		update();
		draw();
	}
}

void Game::update() {
	for (int i = 0; i < 4; i++)
		player[i].update(this);

	for (int i = 0; i < MAX_SCORE_NUM; i++) {
		score[i].update(this);
	}
	level.update();
}

static pvr_poly_hdr_t user_clip = {
        PVR_CMD_USERCLIP, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000
};
extern matrix_t projection_matrix;
void Game::draw() {

	static char buf[256];
	polysent = 0;
	vertextest = 0;

	// begin render with TA
	pvr_wait_ready();
	pvr_scene_begin();
	pvr_list_begin(PVR_LIST_OP_POLY);
	uint64 start = timer_ms_gettime64();

	// draw..
	for (int i = 0; i < 4; i++) {
		q3dMatrixIdentity();
		q3dMatrixTranslate(0,-4.5f-player[i].camheight-player[i].camadd, player[i].camzoom /*10*/);
		q3dMatrixRotateX(player[i].camagl);
		q3dMatrixRotateY(player[i].rotation.y);
		q3dMatrixTranslate(-player[i].position.x, 0/*-player[i].position.y*/,-player[i].position.z);

		q3dMatrixStore(&_q3dMatrixCamera);
		if (i == 0) {
			// upper left
			screen_matrix[3][0] = 1 * 640 / 4.0f;
			screen_matrix[3][1] = 1 * 480 / 4.0f;
			user_clip.d1 = 0;
			user_clip.d2 = 0;
			user_clip.d3 = (640/2)/32-1;
			user_clip.d4 = (480/2)/32-1;
		} else if (i == 1) {
			// upper right
			screen_matrix[3][0] = 640 * 3 / 4.0f;
			screen_matrix[3][1] = 480 * 1 / 4.0f;
			user_clip.d1 = (640/2)/32;
			user_clip.d2 = 0;
			user_clip.d3 = (4*640/4)/32-1;
			user_clip.d4 = (480/2)/32-1;
		} else if (i == 2) {
			// lower left
			screen_matrix[3][0] = 1 * 640 / 4.0f;
			screen_matrix[3][1] = 3 * 480 / 4.0f;
			user_clip.d1 = 0;
			user_clip.d2 = (480/2)/32;
			user_clip.d3 = (640/2)/32-1;
			user_clip.d4 = (4*480/4)/32-2;
		} else {
			// lower right
			screen_matrix[3][0] = 3 * 640 / 4.0f;
			screen_matrix[3][1] = 3 * 480 / 4.0f;
			user_clip.d1 = (640/2)/32;
			user_clip.d2 = (480/2)/32;
			user_clip.d3 = (4*640/4)/32-1;
			user_clip.d4 = (4*480/4)/32-2;
		}
		pvr_prim(&user_clip, sizeof(pvr_poly_hdr_t));
		q3dMatrixLoad(&screen_matrix);
		q3dMatrixApply(&projection_matrix);
		q3dMatrixStore(&_q3dMatrixPerspective);

		q3dTypeVertex vert;
		for (int j = 0; j < 4; j++) {
			q3dMatrixLoad(&_q3dMatrixCamera);
			q3dVertexSet3f(&vert, player[j].position.x, player[j].position.y, player[j].position.z);
			mat_trans_single3(vert.x, vert.y, vert.z);
			if (vert.z < 3)
				continue;
			if (vert.z > 100)
				continue;
			if (vert.x > vert.z + 4)
				continue;
			if (vert.x < -vert.z-4)
				continue;

			player[j].draw();
		}
		for (int j = 0; j < MAX_SCORE_NUM; j++) {
			q3dMatrixLoad(&_q3dMatrixCamera);
			q3dVertexSet3f(&vert, score[j].position.x, score[j].position.y, score[j].position.z);
			mat_trans_single3(vert.x, vert.y, vert.z);

			// test if outside viewing frustum
			if (vert.z < 0)
				continue;
			if (vert.z > 100)
				continue;
			if (vert.x > vert.z + 2)
				continue;
			if (vert.x < -vert.z-2)
				continue;

			score[j].draw();
		}

		q3dMatrixIdentity();
		q3dMatrixRotateY(player[i].rotation.y);
		q3dMatrixStore(&_q3dMatrixTemp);
//		if (i == 1) {
			level.draw();
//		}
	}

	uint64 end = timer_ms_gettime64();

	// commit cross
	pvr_prim(&crossHeader, sizeof(pvr_poly_hdr_t));
	pvr_vertex_t vert;
	vert.argb = PVR_PACK_COLOR(1.0f, 0.0f, 0.0f, 0.0f);
	vert.z = 1 / 0.01f;

	// vertical line
	vert.flags = PVR_CMD_VERTEX;
	vert.x = 640/2-1; vert.y = 480-32;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	/*vert.x = 640/2-1;*/ vert.y = 0;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	vert.x = 640/2+1; vert.y = 480-32;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	vert.flags = PVR_CMD_VERTEX_EOL;
	/*vert.x = 448+xadd;*/ vert.y = 0;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	// horizontal line
	vert.flags = PVR_CMD_VERTEX;
	vert.x = 0; vert.y = (480-32)/2+1;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	/*vert.x = 640/2-1;*/ vert.y = (480-32)/2-1;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	vert.x = 640; vert.y = (480-32)/2+1;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	vert.flags = PVR_CMD_VERTEX_EOL;
	/*vert.x = 448+xadd;*/ vert.y = (480-32)/2-1;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	// todo: draw health, score, frags, etc-display

	pvr_list_finish();

	pvr_list_begin(PVR_LIST_TR_POLY);

	w.y = 60;
	w.x = 20;
	w.z = 100;
	plx_fcxt_begin(fcxt);
	plx_fcxt_setsize(fcxt, 16);
	plx_fcxt_setcolor4f(fcxt, 1.0, 1.0, 1.0, 1.0);

	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "%d", player[0].score);
	plx_fcxt_draw(fcxt, buf);

#ifdef BETA
	w.y += 32;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "time: %d", end - start);
	plx_fcxt_draw(fcxt, buf);

	w.y += 32;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "polygons: %d", polysent);
	plx_fcxt_draw(fcxt, buf);
/*
	w.y += 32;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "tests: %d", vertextest);
	plx_fcxt_draw(fcxt, buf);

	w.y += 32;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "pos: (%f, %f, %f)", player[0].position.x, player[0].position.y, player[0].position.z);
	plx_fcxt_draw(fcxt, buf);
	w.y += 32;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "rot: (%f, %f, %f)", player[0].rotation.x, player[0].rotation.y, player[0].rotation.z);
	plx_fcxt_draw(fcxt, buf);
*/
	w.y = 60;
#endif

	w.x += 320;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "%d", player[1].score);
	plx_fcxt_draw(fcxt, buf);

	w.y += 240-32;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "%d", player[3].score);
	plx_fcxt_draw(fcxt, buf);

	w.x -= 320;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "%d", player[2].score);
	plx_fcxt_draw(fcxt, buf);

	float sec = gametime - (GAMELENGTH * 1000 * 60 - 10 * 1000);
	sec /= 1000.0f;
	if (sec > 0 && sec <= 11) {
		int sec2 = (int) sec;
		w.x = 320-32;
		w.y = 240;
		float alpha = 1 - (sec - sec2);
		if (alpha < 0) alpha = 0;
		sec2 = 10 - sec2;

		plx_fcxt_setcolor4f(fcxt, alpha, 0.75, 0, 0);
		plx_fcxt_setsize(fcxt, 128);
		plx_fcxt_setpos_pnt(fcxt, &w);
		sprintf(buf, "%d", sec2);
		plx_fcxt_draw(fcxt, buf);
	}

	if (sec > 10) {
		float alpha = (sec - 10) * 0.125;
		if (alpha > 0.75) alpha = 0.75;

		int win = -1;
		int winscore = -100000;
		for (int i = 0; i < 4; i++) {
			if (player[i].score > winscore) {
				winscore = player[i].score;
				win = i;
			}
		}
		w.x = 20;
		w.y = 240;
		w.z = 1001;

		plx_fcxt_setcolor4f(fcxt, alpha, 1.0, 1.0, 1.0);
		plx_fcxt_setsize(fcxt, 64);
		plx_fcxt_setpos_pnt(fcxt, &w);
		sprintf(buf, "player %d wins!", win+1);
		plx_fcxt_draw(fcxt, buf);
		plx_fcxt_end(fcxt);

		// fade out the rest
		pvr_prim(&fadeHeader, sizeof(pvr_poly_hdr_t));
		pvr_dr_state_t state;
		pvr_dr_init(state);
		pvr_vertex_t *vert;


		uint32 color = PVR_PACK_COLOR(alpha, 0.0f, 0.0f, 0.0f);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->argb = color;
		vert->z = 1000;
		vert->x = 0; vert->y = 480;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->argb = color;
		vert->z = 1000;
		vert->x = 0; vert->y = 0;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->argb = color;
		vert->z = 1000;
		vert->x = 640; vert->y = 480;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->argb = color;
		vert->flags = PVR_CMD_VERTEX_EOL;
		vert->z = 1000;
		vert->x = 640; vert->y = 0;
		pvr_dr_commit(vert);

	} else {
		plx_fcxt_end(fcxt);
	}

	if (sec > 14) {
		MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)
			if (st->buttons & CONT_START || st->buttons & CONT_A) {
				done = true;
			}
		MAPLE_FOREACH_END()
	}
	pvr_list_finish();

	pvr_scene_finish();
}

