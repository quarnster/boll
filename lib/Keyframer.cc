#include "Keyframer.h"

void KeyFramer::getKey(KeyFrame *keys, uint32 length, Point3D *target, float time) {

	if (time < 0) time = 0;
	if (length == 1) target->set(keys[0].x, keys[0].y, keys[0].z);

	int key = 0;
	int len = length - 1;

	// which key?
	for (; key < len; key++) {
		if (key != 0 && keys[key].time > time) {
			key--;
			break;
		}
	}
	if (key == len) key--;

	int key1 = key + 1;

	// how far are we in this key?
	float t = ((time - keys[key].time) / (float) (keys[key1].time - keys[key].time));

	if (t > 1) t = 1;

	float t2 = t * t;
	float t3 = t2 * t;
	float fir = 2*t3-3*t2+1;
	float sec = t3-2*t2+t;
	float tri = -2*t3+3*t2;

	*target = keys[key] * fir + keys[key].tangent * sec + keys[key1] * tri + keys[key1].tangent * (t3-t2);
//	target->x = (float) (keys[key].x * fir + keys[key].tangent.x * sec + keys[key1].x * tri + keys[key1].tangent.x * (t3-t2));
//	target->y = (float) (keys[key].y * fir + keys[key].tangent.y * sec + keys[key1].y * tri + keys[key1].tangent.y * (t3-t2));
//	target->z = (float) (keys[key].z * fir + keys[key].tangent.z * sec + keys[key1].z * tri + keys[key1].tangent.z * (t3-t2));

}


void KeyFramer::generate(KeyFrame *track, uint32 length) {
	for (uint32 i = 0; i < length; i++) {
		getv(track, length, &track[i].tangent, false, i);
	}
}

void KeyFramer::getv(KeyFrame *track, uint32 length, Point3D *target, bool anStuff, uint32 n) {
	KeyFrame *kn_1, *kn, *kn1;
//	Point3D *pn_1, *pn, *pn1;

	kn = &track[n];
//	pn = kn;

//		if (sel == point) return pn;

	if (n == 0) {
		//first key
		kn1 = &track[1];
//		pn1 = kn1;

		if (length == 2) {
			//2 keys

			*target = (*kn1 - *kn) * (1.0f - kn->tension);
			return;
		}

//			if (mode != 3) {
			//first key, no loop
			Point3D tmp;
			getv(track, length, &tmp, true, 1);

			*target = (((*kn1 - *kn) * 1.5f) - (tmp * 0.5f)) * (1.0f - kn->tension);

			return;
//			} else {
			//first key, loop
//				kn_1= track[track.length-2];
//			}
	} else if (n == length-1) {
		//last key
		kn_1 = &track[n-1];
//		pn_1 = kn_1;

		if (length == 2) {
			//2 keys
			*target = (*kn - *kn_1) * (1.0f - kn->tension);
			return;
		}
//			if (mode != 3) {
			//last key, no loop
			Point3D tmp;
			getv(track, length, &tmp, true, n - 1);

			*target = (((*kn - *kn_1) * 1.5f) - (tmp * 0.5f)) * (1.0f - kn->tension);
			return;
//			} else {
			//last key, loop
//				kn1 = track[1];
//			}
	} else {
		//middle keys
		kn_1= &track[n-1];
		kn1 = &track[n+1];
	}
//	pn_1= kn_1;
//	pn1 = kn1;

	float f = (anStuff == true) ? 0.5f : -0.5f;

	Point3D g1 = (*kn - *kn_1) * (1.0f + kn->bias);
	Point3D g2 = (*kn1 - *kn) * (1.0f - kn->bias);
	*target = (g1 + ((g2-g1)*(0.5f + f * kn->continuity)))*(1.0f - kn->tension);
}
