#include <kos.h>
#include "ntscmenu.h"

extern pvr_poly_cxt_t loadImage(char *, int);
NtscMenu::NtscMenu() {
	pvr_poly_cxt_t cxt = loadImage("palntsc.png", PVR_LIST_TR_POLY);
	pvr_poly_compile(&header, &cxt);
}

NtscMenu::~NtscMenu() {
	pvr_mem_free(addr);
}

void NtscMenu::showMenu() {
	int choice = 0;
	bool done = false;
	float time = 0;
	float framerate = 1.0;

	while (!done) {
		MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)
			if (st->buttons & CONT_START || st->buttons & CONT_A)
				done = true;
			// TODO: change item on up/down
			if (st->joyy < -64 && choice != 0) {
				choice = 0;
				framerate = 1.0f;
				vid_set_mode(DM_640x480_PAL_IL,PM_RGB565);
			} else if (st->joyy > 64 && choice != 1) {
				choice = 1;
				framerate = 50.0f / 60.0f;
				vid_set_mode(DM_640x480_NTSC_IL, PM_RGB565);
			}
			if (st->joy2y != 0) {
				printf("!= 0: %d\n", st->joy2y);
			}
		MAPLE_FOREACH_END()

		// begin render with TA
		pvr_scene_begin();
		pvr_list_begin(PVR_LIST_TR_POLY);

		pvr_prim(&header, sizeof(pvr_poly_hdr_t));

		pvr_vertex_t vert;
		vert.argb = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vert.z = 1 / 1.10f;

		float xadd = fsin(time * 1.1) * 10;
		float yadd = fsin(time * 1.3) * 10;

		if (choice == 1) {
			xadd = 0;
			yadd = 0;
		}

		vert.flags = PVR_CMD_VERTEX;
		vert.u = 0; vert.v = 0.5;
		vert.x = 192-xadd; vert.y = 272-64+yadd;
		pvr_prim(&vert, sizeof(pvr_vertex_t));

		vert.u = 0; vert.v = 0;
		vert.x = 192-xadd; vert.y = 208-64-yadd;
		pvr_prim(&vert, sizeof(pvr_vertex_t));

		vert.u = 1; vert.v = 0.5;
		vert.x = 448+xadd; vert.y = 272-64+yadd;
		pvr_prim(&vert, sizeof(pvr_vertex_t));

		vert.flags = PVR_CMD_VERTEX_EOL;
		vert.u = 1; vert.v = 0;
		vert.x = 448+xadd; vert.y = 208-64-yadd;
		pvr_prim(&vert, sizeof(pvr_vertex_t));


		// ntsc
		vert.argb = PVR_PACK_COLOR(1.0f, 1.0f, 0.0f, 0.0f);
		vert.z = 1 / 1.10f;

		xadd = fsin(time * 1.1) * 10;
		yadd = fsin(time * 1.3) * 10;

		if (choice == 0) {
			xadd = 0;
			yadd = 0;
		}

		vert.flags = PVR_CMD_VERTEX;
		vert.u = 0; vert.v = 1;
		vert.x = 192-xadd; vert.y = 272+64+yadd;
		pvr_prim(&vert, sizeof(pvr_vertex_t));

		vert.u = 0; vert.v = 0.5;
		vert.x = 192-xadd; vert.y = 208+64-yadd;
		pvr_prim(&vert, sizeof(pvr_vertex_t));

		vert.u = 1; vert.v = 1;
		vert.x = 448+xadd; vert.y = 272+64+yadd;
		pvr_prim(&vert, sizeof(pvr_vertex_t));

		vert.flags = PVR_CMD_VERTEX_EOL;
		vert.u = 1; vert.v = 0.5;
		vert.x = 448+xadd; vert.y = 208+64-yadd;
		pvr_prim(&vert, sizeof(pvr_vertex_t));


		pvr_list_finish();

		pvr_scene_finish();
		pvr_wait_ready();

		time += 1 / 10.0f * framerate;
	}
}

