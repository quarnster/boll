#include "disclaimer.h"
#include "resources.h"

#define LINES 12

static int width[LINES];
static float fontSize = 24;
static char disc_text[LINES][45] = {
	"Obviously, we are not quite",
	"finished yet... Read \"gamedoc.doc\"",
	"for a more complete picture of our",
	"vision with this game.",
	"",
	"This game needs you!",
	"We are very interested in your",
	"opinion about what could be done",
	"better (and how), what to remove,",
	"what to definately keep, etc etc.",
	"See readme.txt for contact",
	"information."
};

Disclaimer::Disclaimer() {
	for (int i = 0; i < LINES; i++) {
		float l, u, d, r;
		plx_fcxt_str_metrics(fcxt, disc_text[i], &l, &u, &r, &d);
		width[i] = 640 / 2 - (int) ((l+r) / 2);
	}
}
void Disclaimer::show() {
	pvr_scene_begin();
	pvr_list_begin(PVR_LIST_TR_POLY);

	// draw credits text
	plx_fcxt_setcolor4f(fcxt, 1, 1, 1, 1);

	plx_fcxt_begin(fcxt);

	for (int i = 0; i < LINES; i++) {
		float y = (int) (i * fontSize + 240 - (fontSize / 2)*LINES);

		if (i == 5)
			plx_fcxt_setcolor4f(fcxt, 1, 1, 0, 0);
		else
			plx_fcxt_setcolor4f(fcxt, 1, 1, 1, 1);
		plx_fcxt_setpos(fcxt, width[i], y, 2);
		plx_fcxt_draw(fcxt, disc_text[i]);
	}

	plx_fcxt_end(fcxt);

	pvr_list_finish();

	pvr_scene_finish();

}

