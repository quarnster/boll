#include <string.h>
#include <stdlib.h>
#include "game.h"
#include "lib/primitives.h"

#include <plx/font.h>
#include <plx/list.h>
#include <plx/dr.h>

static point_t w;
extern uint32 polysent;
extern uint32 vertextest;

#define GAMELENGTH 5.0

q3dTypePolyhedron *sphere;
q3dTypePolyhedron *scorePolyhedron;
q3dTypePolyhedron *powerupPolyhedron;
q3dTypeFiller fillerPlayers;
q3dTypeFiller fillerLevel;
q3dTypeFiller fillerScore;
q3dTypeFiller fillerPowerup;

q3dTypeMatrix screen_matrix __attribute__((aligned(32))) = {
    { 640/4.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 480 / 4.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

Game::Game() {
	countdown = 1;
	soundtrack = INGAMETRACK1;
	gameended = false;
	type = GET_HIM;

	q3dMatrixInit();

	sphere = generateSphere(10, 10);
	scorePolyhedron = generateSphere(3, 3);
	powerupPolyhedron = generateCube(1);

	q3dPolyhedronCompile(sphere);
	q3dPolyhedronCompile(scorePolyhedron);
	q3dPolyhedronCompile(powerupPolyhedron);

	q3dFillerTextureInit(&fillerPlayers);
	fillerPlayers.defaultCxt = loadImage("player.png", PVR_LIST_OP_POLY);
	fillerPlayers.defaultCxt.gen.clip_mode = PVR_USERCLIP_INSIDE;
//	fillerPlayers.defaultCxt.gen.fog_type = PVR_FOG_TABLE;
	pvr_poly_compile(&fillerPlayers.defaultHeader, &fillerPlayers.defaultCxt);

	q3dFillerTextureInit(&fillerLevel);
	fillerLevel.defaultCxt = loadImage("ground.png", PVR_LIST_OP_POLY);
	fillerLevel.defaultCxt.gen.clip_mode = PVR_USERCLIP_INSIDE;
	fillerLevel.defaultCxt.gen.fog_type = PVR_FOG_TABLE;
	pvr_poly_compile(&fillerLevel.defaultHeader, &fillerLevel.defaultCxt);

	q3dFillerStandardInit(&fillerScore);
	fillerScore.defaultCxt.gen.clip_mode = PVR_USERCLIP_INSIDE;
//	fillerScore.defaultCxt.gen.fog_type = PVR_FOG_TABLE;
	pvr_poly_compile(&fillerScore.defaultHeader, &fillerScore.defaultCxt);

	q3dFillerStandardInit(&fillerPowerup);
	fillerPowerup.defaultCxt.gen.clip_mode = PVR_USERCLIP_INSIDE;
//	fillerPowerup.defaultCxt.gen.fog_type = PVR_FOG_TABLE;
	pvr_poly_compile(&fillerPowerup.defaultHeader, &fillerPowerup.defaultCxt);

	q3dCameraInit(&cam);
	cam._pos.y = 5;
	cam._pos.z = -5;

	player = new Player[4];
	for (int i = 0; i < 4; i++)
		player[i].setController(i);

	player[0].position.set( 0,  0,  +2); q3dColorSet3f(&player[0].baseColor, 1, 0, 0);
	player[1].position.set( 0,  10, +2); q3dColorSet3f(&player[1].baseColor, 0, 1, 0);
	player[2].position.set(-2,  2, -2); q3dColorSet3f(&player[2].baseColor, 0, 0, 1);
	player[3].position.set(+2,  2, -2); q3dColorSet3f(&player[3].baseColor, 1, 1, 0);

	for (int i = 0; i < MAX_POWERUP_NUM; i++)
		powerup[i].positionate();

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

	soundtrack = INGAMETRACK1 + rand() % INGAMETRACKNUM;
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

extern bool done2;
extern uint64 gametime;
void Game::reset() {
	frame = 0;
	endtrack = false;
	done = false;
	gamestart = timer_ms_gettime64();
	gametime = 0;
	gameended = false;

	for (int i = 0; i < 4; i++) {
		player[i].setScore(0);
		player[i].active = false;
	}

	player[0].position.set( 0,  0,  +2);
	player[1].position.set( 0,  10, +2);
	player[2].position.set(-2,  2, -2);
	player[3].position.set(+2,  2, -2);

	player[rand() % 4].active = true;

	soundtrack++;
	if (soundtrack >= INGAMETRACK1 + INGAMETRACKNUM)
		soundtrack = INGAMETRACK1;

	cddaPlay(soundtrack);
}

//extern bool done;
void Game::run() {
	reset();
	while (!done2 && !done /*&& (gametime < 60 * 1000 * GAMELENGTH)*/) {
		// TODO: check for exit.. ?
		gametime = timer_ms_gettime64() - gamestart;
		if (!endtrack && gametime >= 60 * 1000 * GAMELENGTH - 12 * 1000) {
			endtrack = true;
			cdrom_cdda_play(SCORETRACK, SCORETRACK, 0, CDDA_TRACKS);
		}

		if (!gameended && gametime >= 60 * 1000 * GAMELENGTH) {
			gameended = true;
		}

		update();
		draw();
		frame++;
	}
}

void Game::update() {
	for (int i = 0; i < 4; i++)
		player[i].update(this);

	for (int i = 0; i < MAX_SCORE_NUM; i++) {
		score[i].update(this);
	}
	for (int i = 0; i < MAX_POWERUP_NUM; i++) {
		powerup[i].update(this);
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
		player[i].camera.toMatrix();
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
//			if (vert.z > 150)
//				continue;
			if (vert.x > vert.z + 4)
				continue;
			if (vert.x < -vert.z - 4)
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
			if (vert.x < -vert.z - 2)
				continue;

			score[j].draw();
		}
		for (int j = 0; j < MAX_POWERUP_NUM; j++) {
			q3dMatrixLoad(&_q3dMatrixCamera);
			q3dVertexSet3f(&vert, powerup[j].position.x, powerup[j].position.y, powerup[j].position.z);
			mat_trans_single3(vert.x, vert.y, vert.z);

			// test if outside viewing frustum
			if (vert.z < 0)
				continue;
//			if (vert.z > 150)
//				continue;
			if (vert.x > vert.z + 2.5)
				continue;
			if (vert.x < -vert.z - 2.5)
				continue;

			powerup[j].draw();
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
	sprintf(buf, "%d", player[0].getCurrentScore());
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

	w.y += 32;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "tests: %d", vertextest);
	plx_fcxt_draw(fcxt, buf);
/*
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
	sprintf(buf, "%d", player[1].getCurrentScore());
	plx_fcxt_draw(fcxt, buf);

	w.y += 240-32;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "%d", player[3].getCurrentScore());
	plx_fcxt_draw(fcxt, buf);

	w.x -= 320;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "%d", player[2].getCurrentScore());
	plx_fcxt_draw(fcxt, buf);

	w.x = 320 - 100;
	w.y = 448;
	w.z = 1000.1f;
	plx_fcxt_setpos_pnt(fcxt, &w);
	plx_fcxt_setsize(fcxt, 32);
	if (type == GET_HIM) {
		plx_fcxt_draw(fcxt, "Get him!");
	} else if (type == WATCH_OUT) {
		plx_fcxt_draw(fcxt, "Watch out!");
	}

	long lsec = (gametime - (GAMELENGTH * 1000 * 60 - 10 * 1000));
	float sec = lsec / 1000.0f;
	if (sec > 0 && sec < 10) {
		int sec2 = (int) sec;
		if (countdown != sec2) {
			countdown = sec2;
			snd_sfx_play(sounds[COUNTDOWN1+9-countdown], 255, 128);
		}

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
		float alpha = (sec - 20.7) * 0.5;
		if (alpha > 1.0) alpha = 1.0;
		else if (alpha < 0) alpha = 0;

		int win = -1;
		int winscore = -100000;
		for (int i = 0; i < 4; i++) {
			int score = player[i].getScore();
			if (score > winscore) {
				winscore = score;
				win = i;
			}
		}
		w.x = 180-4;
		w.y = 240;
		w.z = 1001;

		plx_fcxt_setsize(fcxt, 64);
		plx_fcxt_setcolor4f(fcxt, 1.0-alpha, 1.0, 1.0, 1.0);
		plx_fcxt_setpos_pnt(fcxt, &w);
		plx_fcxt_draw(fcxt, "the end");

		if (alpha > 0.75) alpha = 0.75;
		else if (alpha < 0) alpha = 0;

		w.x = 30;
//		w.y += 64;
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
		vert->z = 1000.5;
		vert->x = 0; vert->y = 480;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->argb = color;
		vert->z = 1000.5;
		vert->x = 0; vert->y = 0;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->argb = color;
		vert->z = 1000.5;
		vert->x = 640; vert->y = 480;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->argb = color;
		vert->flags = PVR_CMD_VERTEX_EOL;
		vert->z = 1000.5;
		vert->x = 640; vert->y = 0;
		pvr_dr_commit(vert);

	} else {
		plx_fcxt_end(fcxt);
	}

	if (sec > 22) {
		MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)
			if (st->buttons & CONT_START || st->buttons & CONT_A) {
				done = true;
			}
		MAPLE_FOREACH_END()
	}
	pvr_list_finish();

	pvr_scene_finish();
}

