#ifndef __INCLUDED_Q3D_KEYFRAMER_H
#define __INCLUDED_Q3D_KEYFRAMER_H


#include <kos.h>
#include "Keyframe.h"

class KeyFramer {
public:
	static void generate(KeyFrame *track, uint32 length);
	static void getv(KeyFrame *track, uint32 length, Point3D *target, bool anStuff, uint32 n);

	static void getKey(KeyFrame *keys, uint32 length, Point3D *target, float time);
};


#endif

