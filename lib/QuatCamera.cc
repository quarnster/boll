#include <math.h>
#include "QuatCamera.h"

QuatCamera::QuatCamera() {
	q3dQuaternionInit(&qRot);
}

#define DELTA 1.0 / 32
inline void q3dQuaternionSLERP(q3dTypeQuaternion *from, q3dTypeQuaternion *to, float t, q3dTypeQuaternion *res) {
	float	to1[4];
	double	omega, cosom, sinom, scale0, scale1;

	// calc cosine
	cosom = from->x * to->x + from->y * to->y + from->z * to->z + from->w * to->w;

	// adjust signs (if necessary)
	if (cosom < 0.0) {
		cosom = -cosom;
		to1[0] = -to->x;
		to1[1] = -to->y;
		to1[2] = -to->z;
		to1[3] = -to->w;
	} else {
		to1[0] = to->x;
		to1[1] = to->y;
		to1[2] = to->z;
		to1[3] = to->w;
	}

	// calculate coefficients

	if ((1.0 - cosom) > DELTA) {
		// standard case (slerp)
		omega = acos(cosom);
		sinom = fsin(omega);
		scale0 = fsin((1.0 - t) * omega) / sinom;
		scale1 = fsin(t * omega) / sinom;
	} else {
		// "from" and "to" are very close
		// ... so we can do a linear interpolation
		scale0 = 1.0 - t;
		scale1 = t;
	}

	// calculate final values
	res->x = scale0 * from->x + scale1 * to1[0];
	res->y = scale0 * from->y + scale1 * to1[1];
	res->z = scale0 * from->z + scale1 * to1[2];
	res->w = scale0 * from->w + scale1 * to1[3];
}

void QuatCamera::toMatrix() {
	float a = rotation.x * 0.5;
	static q3dTypeQuaternion qx;
	qx.w = cos(a);
	qx.x = sin(a);
	qx.y = qx.z = 0;

	a = rotation.y * 0.5;
	static q3dTypeQuaternion qy;
	qy.w = cos(a);
	qy.x = 0;
	qy.y = sin(a);
	qy.z = 0;

	q3dQuaternionNormalize(&qx);
	q3dQuaternionNormalize(&qy);
/*
Qx = [ cos(a/2), (sin(a/2), 0, 0)]
Qy = [ cos(b/2), (0, sin(b/2), 0)]
Qz = [ cos(c/2), (0, 0, sin(c/2))]

And the final quaternion is obtained by Qx * Qy * Qz.
*/
	q3dQuaternionMul(&qy, &qx);

	static q3dTypeQuaternion slerp;
	q3dQuaternionSLERP(&qRot, &qy, 0.1, &slerp);
	q3dQuaternionSetV(&qRot, &slerp);


	q3dQuaternionToMatrix(&qRot);

	q3dMatrixIdentity();
	q3dMatrixTranslate(0, -4.5f, zoom);
	q3dMatrixApply(&_q3dMatrixTemp);
//	q3dMatrixRotateX(rotation.x);
//	q3dMatrixRotateY(rotation.y);
	q3dMatrixTranslate(-position.x, -position.y,-position.z);

	q3dMatrixStore(&_q3dMatrixCamera);
}
