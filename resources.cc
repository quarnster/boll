#include "resources.h"

sfxhnd_t sounds[SOUND_NUM];
plx_font_t *fnt;
plx_fcxt_t *fcxt;


void loadSounds() {
	// load sounds
	sounds[BOUNCE] = snd_sfx_load("bounce.wav");
	sounds[BOUNCE2] = snd_sfx_load("bounce2.wav");
	sounds[JUMP] = snd_sfx_load("jump.wav");
	sounds[FALL] = snd_sfx_load("fall.wav");
}

void freeSounds() {
	printf("TODO: free sounds...\n");
}

void loadFont() {
	fnt = plx_font_load("font.txf");
	fcxt = plx_fcxt_create(fnt, PVR_LIST_TR_POLY);
}

void freeFont() {
	plx_fcxt_destroy(fcxt);
	plx_font_destroy(fnt);
}

void loadResources() {
	loadSounds();
	loadFont();
}

void freeResources() {
	freeSounds();
	freeFont();
}

pvr_poly_cxt_t loadImage(char *name, int list) {
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
		list,
		PVR_TXRFMT_RGB565 | PVR_TXRFMT_TWIDDLED,
		width,
		height,
		ptr,
		PVR_FILTER_BILINEAR
	);

	return cxt;
}

