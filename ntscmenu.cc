#include <kos.h>
#include "ntscmenu.h"

extern pvr_poly_cxt_t loadImage(char *, int);
NtscMenu::NtscMenu() {
	pvr_poly_cxt_t cxt = loadImage("palntsc.png", PVR_LIST_OP_POLY);
	pvr_poly_compile(&header, &cxt);
}

NtscMenu::~NtscMenu() {
	printf("TODO: free pal/ntsc-image\n");
//	pvr_mem_free(addr);
}

void NtscMenu::showMenu() {
	int choice = 0;
	bool done = false;
	float time = 0;
	float framerate = 1.0;

	pvr_dr_state_t state;
	pvr_dr_init(state);
	pvr_vertex_t *vert;

	while (!done) {
		MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)
			if (st->buttons & CONT_START || st->buttons & CONT_A)
				done = true;
			if ((st->buttons & CONT_DPAD_UP || st->joyy < -64) && choice != 0) {
				choice = 0;
				framerate = 1.0f;
				vid_set_mode(DM_640x480_PAL_IL,PM_RGB565);
			} else if ((st->buttons & CONT_DPAD_DOWN || st->joyy > 64) && choice != 1) {
				choice = 1;
				framerate = 50.0f / 60.0f;
				vid_set_mode(DM_640x480_NTSC_IL, PM_RGB565);
			}
		MAPLE_FOREACH_END()


		// begin render with TA
		pvr_wait_ready();
		pvr_scene_begin();
		pvr_list_begin(PVR_LIST_OP_POLY);

		pvr_prim(&header, sizeof(pvr_poly_hdr_t));


		float xadd = fsin(time * 1.1) * 10;
		float yadd = fsin(time * 1.3) * 10;

		if (choice == 1) {
			xadd = 0;
			yadd = 0;
		}

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->argb = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vert->z = 1 / 1.10f;
		vert->u = 0; vert->v = 0.5;
		vert->x = 192-xadd; vert->y = 272-64+yadd;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->argb = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vert->z = 1 / 1.10f;
		vert->u = 0; vert->v = 0;
		vert->x = 192-xadd; vert->y = 208-64-yadd;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->flags = PVR_CMD_VERTEX;
		vert->argb = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vert->z = 1 / 1.10f;
		vert->u = 1; vert->v = 0.5;
		vert->x = 448+xadd; vert->y = 272-64+yadd;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->argb = PVR_PACK_COLOR(1.0f, 1.0f, 1.0f, 1.0f);
		vert->flags = PVR_CMD_VERTEX_EOL;
		vert->z = 1 / 1.10f;
		vert->u = 1; vert->v = 0;
		vert->x = 448+xadd; vert->y = 208-64-yadd;
		pvr_dr_commit(vert);


		// ntsc
		xadd = fsin(time * 1.1) * 10;
		yadd = fsin(time * 1.3) * 10;

		if (choice == 0) {
			xadd = 0;
			yadd = 0;
		}

		vert = pvr_dr_target(state);
		vert->argb = PVR_PACK_COLOR(1.0f, 1.0f, 0.0f, 0.0f);
		vert->z = 1 / 1.10f;
		vert->flags = PVR_CMD_VERTEX;
		vert->u = 0; vert->v = 1;
		vert->x = 192-xadd; vert->y = 272+64+yadd;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->argb = PVR_PACK_COLOR(1.0f, 1.0f, 0.0f, 0.0f);
		vert->z = 1 / 1.10f;
		vert->flags = PVR_CMD_VERTEX;
		vert->u = 0; vert->v = 0.5;
		vert->x = 192-xadd; vert->y = 208+64-yadd;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->argb = PVR_PACK_COLOR(1.0f, 1.0f, 0.0f, 0.0f);
		vert->z = 1 / 1.10f;
		vert->flags = PVR_CMD_VERTEX;
		vert->u = 1; vert->v = 1;
		vert->x = 448+xadd; vert->y = 272+64+yadd;
		pvr_dr_commit(vert);

		vert = pvr_dr_target(state);
		vert->argb = PVR_PACK_COLOR(1.0f, 1.0f, 0.0f, 0.0f);
		vert->z = 1 / 1.10f;
		vert->flags = PVR_CMD_VERTEX_EOL;
		vert->u = 1; vert->v = 0.5;
		vert->x = 448+xadd; vert->y = 208+64-yadd;
		pvr_dr_commit(vert);


		pvr_list_finish();

		pvr_scene_finish();

		time += 1 / 10.0f * framerate;
	}
}

