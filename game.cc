#include <string.h>
#include "game.h"
#include "lib/primitives.h"

#include <plx/font.h>
#include <plx/list.h>
#include <plx/dr.h>

static plx_font_t *fnt;
static plx_fcxt_t *fcxt;
static point_t w;
extern uint32 polysent;
extern uint32 vertextest;


q3dTypePolyhedron *sphere;
q3dTypeFiller fillerPlayers;
q3dTypeFiller fillerLevel;

q3dTypeMatrix screen_matrix __attribute__((aligned(32))) = {
    { 480/4.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 480 / 4.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

sfxhnd_t sounds[SOUND_NUM];

extern pvr_poly_cxt_t loadImage(char *, int);
Game::Game() {
	q3dMatrixInit();

	sphere = generateSphere();
//	q3dFillerCellInit(&fillerPlayers);
	q3dFillerStandardInit(&fillerPlayers);
//	fillerPlayers.defaultCxt.gen.clip_mode = PVR_USERCLIP_INSIDE;
//	fillerPlayers.defaultCxt.gen.fog_type = PVR_FOG_TABLE;
	pvr_poly_compile(&fillerPlayers.defaultHeader, &fillerPlayers.defaultCxt);

	q3dFillerStandardInit(&fillerLevel);
	fillerLevel.defaultCxt = loadImage("buzz.png", PVR_LIST_OP_POLY);
	fillerLevel.defaultCxt.gen.clip_mode = PVR_USERCLIP_DISABLE; //PVR_USERCLIP_INSIDE;
	fillerLevel.defaultCxt.gen.fog_type = PVR_FOG_TABLE;
	pvr_poly_compile(&fillerLevel.defaultHeader, &fillerLevel.defaultCxt);

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

	// load sounds
	sounds[BOUNCE] = snd_sfx_load("bounce.wav");
	sounds[BOUNCE2] = snd_sfx_load("bounce2.wav");
	sounds[JUMP] = snd_sfx_load("jump.wav");
	sounds[FALL] = snd_sfx_load("fall.wav");

	// load debug-font
	fnt = plx_font_load("font.txf");
	fcxt = plx_fcxt_create(fnt, PVR_LIST_TR_POLY);
	plx_fcxt_setcolor4f(fcxt, 1, 1, 1, 1);

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

extern bool done;
void Game::run() {
	while (!done/*true*/) {
		// TODO: check for exit.. ?
		update();
		draw();
	}
}

void Game::update() {
	for (int i = 0; i < 4; i++)
		player[i].update(this);

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
		q3dMatrixTranslate(0,-8,10);
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

		for (int j = 0; j < 4; j++) {
			// TODO: camera stuff..
			player[j].draw();
		}

		q3dMatrixIdentity();
		q3dMatrixRotateY(player[i].rotation.y);
		q3dMatrixStore(&_q3dMatrixTemp);
		if (i == 0) {
			level.draw();
		}
	}

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

	w.y = 32;
	plx_fcxt_begin(fcxt);

	w.y += 32;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "time: %d", timer_ms_gettime64() - start);
	plx_fcxt_draw(fcxt, buf);

	w.y += 32;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "polygons: %d", polysent);
	plx_fcxt_draw(fcxt, buf);

	w.y += 32;
	plx_fcxt_setpos_pnt(fcxt, &w);
	sprintf(buf, "tests: %d", vertextest);
	plx_fcxt_draw(fcxt, buf);

	plx_fcxt_end(fcxt);

	pvr_list_finish();

	pvr_scene_finish();
}

