#include "resources.h"

sfxhnd_t sounds[SOUND_NUM];
plx_font_t *fnt;
plx_fcxt_t *fcxt;


void loadSounds() {
	char buf[10];

	// load sounds
	sounds[BOUNCE] = snd_sfx_load("bounce.wav");
	sounds[JUMP] = snd_sfx_load("jump.wav");
	sounds[FALL] = snd_sfx_load("fall.wav");
	sounds[MENU_CHANGE] = snd_sfx_load("m_change.wav");
	sounds[MENU_SELECT] = snd_sfx_load("m_select.wav");
	sounds[POWERUP_APPEAR] = snd_sfx_load("p_exists.wav");
	sounds[POWERUP_GET1] = snd_sfx_load("p_minus.wav");
	sounds[POWERUP_GET2] = snd_sfx_load("p_plus.wav");
	sounds[CHARGE] = snd_sfx_load("charge.wav");
	sounds[CHARGE_RELEASE] = snd_sfx_load("charge_r.wav");
	sounds[SCORE_PLUS] = snd_sfx_load("s_plus.wav");
	sounds[SCORE_MINUS] = snd_sfx_load("s_minus.wav");

	for (int i = 0; i < 10; i++) {
		sprintf(buf, "%d.wav", i+1);
		sounds[COUNTDOWN1+i] = snd_sfx_load(buf);
	}

	return;
}

void freeSounds() {
	printf("TODO: free sounds...\n");
}

void loadFont() {
	fnt = plx_font_load("font.txf");
	fcxt = plx_fcxt_create(fnt, PVR_LIST_TR_POLY);
}

void cddaPlay(int track) {
	while (cdrom_cdda_play(track, track, 15, CDDA_TRACKS) != ERR_OK)
		cdrom_reinit();
}

void freeFont() {
	plx_fcxt_destroy(fcxt);
	plx_font_destroy(fnt);
}

void loadResources() {
	loadSounds();
//	loadFont();
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

	printf("loading \"%s\"\n", name);
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

