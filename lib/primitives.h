#ifndef __INCLUDED_PRIMITIVES_H
#define __INCLUDED_PRIMITIVES_H

#include <q3d.h>

q3dTypePolyhedron *generateCube(float size);
q3dTypePolyhedron *generateSphere(int nRings, int nSegments);
q3dTypePolyhedron *generateTorus(int n, double r);

#endif
