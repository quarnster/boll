#include <kos.h>
#include "mainmenu.h"

extern plx_fcxt_t *fcxt;

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
//	fnt = plx_font_load("font.txf");
//	fcxt = plx_fcxt_create(fnt, PVR_LIST_TR_POLY);
//	plx_fcxt_setcolor4f(fcxt, 1, 1, 1, 1);
	

	for (int i = 0; i < LINENUM; i++) {
		float l, u, d, r;
		plx_fcxt_str_metrics(fcxt, menu_text[i], &l, &u, &r, &d);
		width[i] = 640 / 2 - (int) ((l+r) / 2);
	}
}

MainMenu::~MainMenu() {
//	plx_fcxt_destroy(fcxt);
//	plx_font_destroy(fnt);
}

int MainMenu::showMenu() {
	int choice = 0;
	uint32 last = 0;

	plx_fcxt_setsize(fcxt, fontSize);

	// TODO: play the right cd-track
	while (true) {
		maple_device_t *dev = maple_enum_dev(0, 0);
		if (dev != NULL && dev->info.functions & MAPLE_FUNC_CONTROLLER) {
			cont_state_t* st = (cont_state_t*) maple_dev_status(dev);
			if (st->buttons & CONT_START || st->buttons & CONT_A) {
				return choice;
			}
			if ((st->buttons & CONT_DPAD_UP) && (last & CONT_DPAD_UP) == 0 && choice != 0) {
				choice--;
			} else if ((st->buttons & CONT_DPAD_DOWN)  && (last & CONT_DPAD_DOWN) == 0 && choice != (LINENUM-1)) {
				choice++;
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

