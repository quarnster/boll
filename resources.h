#ifndef __INCLUDED_RESOURCES_H
#define __INCLUDED_RESOURCES_H

#include <kos.h>
#include <png/png.h>
#include <plx/font.h>
//#include <plx/list.h>
//#include <plx/dr.h>


enum {
	BOUNCE = 0,
	BOUNCE2,
	JUMP,
	FALL,
	SOUND_NUM
};

extern sfxhnd_t sounds[];

pvr_poly_cxt_t loadImage(char *name, int list);

void loadResources();
void freeResources();
void loadSounds();
void freeSounds();
void loadFont();
void freeFont();

extern plx_font_t *fnt;
extern plx_fcxt_t *fcxt;

#endif

