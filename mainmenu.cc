#include <kos.h>
#include "mainmenu.h"

#define LINENUM 4
static int width[LINENUM];

static float fontSize = 24;
static char menu_text[LINENUM][45] = {
	"new game",
	"level editor",
	"ball customizer",
	"credits"
};

MainMenu::MainMenu() {
	for (int i = 0; i < LINENUM; i++) {
		float l, u, d, r;
		plx_fcxt_str_metrics(fcxt, menu_text[i], &l, &u, &r, &d);
		width[i] = 640 / 2 - (int) ((l+r) / 2);
	}
}

MainMenu::~MainMenu() {
}

int MainMenu::showMenu() {
	int choice = 0;
	uint32 last = 0;

	cddaPlay(MENUTRACK);
	plx_fcxt_setsize(fcxt, fontSize);
	maple_device_t *dev = maple_enum_dev(0, 0);
	if (dev != NULL && dev->info.functions & MAPLE_FUNC_CONTROLLER) {
		cont_state_t* st = (cont_state_t*) maple_dev_status(dev);
		last = st->buttons;
	}

	// TODO: play the right cd-track
	while (true) {
		dev = maple_enum_dev(0, 0);
		if (dev != NULL && dev->info.functions & MAPLE_FUNC_CONTROLLER) {
			cont_state_t* st = (cont_state_t*) maple_dev_status(dev);
			if (st->buttons != last) {
				if (st->buttons & CONT_START || st->buttons & CONT_A) {
					snd_sfx_play(sounds[MENU_SELECT], 255, 128);
					return choice;
				}
				if (st->buttons & CONT_DPAD_UP && choice != 0) {
					choice--;
					snd_sfx_play(sounds[MENU_CHANGE], 255, 128);
				} else if (st->buttons & CONT_DPAD_DOWN && choice != (LINENUM-1)) {
					choice++;
					snd_sfx_play(sounds[MENU_CHANGE], 255, 128);
				}
			}
			last = st->buttons;
		}

		// begin rendering
		pvr_wait_ready();
		pvr_scene_begin();

		pvr_list_begin(PVR_LIST_OP_POLY);

	//	q3dColorSet3f(&sphere->material.color, 1.0f, 0.0f, 0.0f);
	//	q3dPolyhedronPaint(sphere, &cam, &sphereFiller);

		pvr_list_finish();

		pvr_list_begin(PVR_LIST_TR_POLY);

		// draw credits text
		plx_fcxt_begin(fcxt);

		for (int i = 0; i < LINENUM; i++) {
			if (choice == i) 
				plx_fcxt_setcolor4f(fcxt, 1.0, 0.75, 0, 0);
			else
				plx_fcxt_setcolor4f(fcxt, 1.0, 0.75, 0.75, 0.75);

			plx_fcxt_setpos(fcxt, width[i], 480/2 - LINENUM*fontSize/2 + i * fontSize, 2);
			plx_fcxt_draw(fcxt, menu_text[i]);
		}

		plx_fcxt_end(fcxt);

		pvr_list_finish();

		pvr_scene_finish();
	}
}

