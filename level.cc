#include "level.h"

Level::Level() {
	vertex = (q3dTypeVertex*) malloc(3*64*64*sizeof(q3dTypeVertex));
	finalVertex = (pvr_vertex_t*) malloc(3*64*64*sizeof(pvr_vertex_t));
}

Level::~Level() {
	if (vertex)
		free(vertex);
	if (finalVertex)
		free(finalVertex);
}

void Level::update() {
}

void Level::draw(q3dTypeCamera *cam) {

	// transform vertices
	q3dCameraUpdate(cam);

	q3dMatrixIdentity();
	q3dMatrixApply(&_q3dMatrixPerspective);
	q3dMatrixApply(&_q3dMatrixCamera);

	q3dMatrixTransform(vertex, (pvr_vertex_t*) &finalVertex[0].x, 3*64*64, sizeof(pvr_vertex_t));

	// draw
	// todo...
//	pvr_prim(&poly->material.header, sizeof(pvr_poly_hdr_t));

}

