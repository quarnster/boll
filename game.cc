#include "game.h"

static q3dTypePolyhedron *generateSphere() {
	q3dTypePolyhedron *sphere = (q3dTypePolyhedron*) malloc(sizeof(q3dTypePolyhedron));
	q3dPolyhedronInit(sphere);

	int nRings = 6;
	int nSegments = 6;

	sphere->vertexLength = ( nRings + 1 ) * ( nSegments + 1 );
	sphere->vertex = (q3dTypeVertex*) malloc(sphere->vertexLength * sizeof(q3dTypeVertex));

	sphere->polygonLength = 1;
	sphere->polygon = (q3dTypePolygon*) malloc(sphere->polygonLength * sizeof(q3dTypePolyhedron));

	q3dPolygonInit(&sphere->polygon[0]);
	sphere->polygon[0].vertexLength = 2 * nRings * ( nSegments + 1 ) ;;
	sphere->polygon[0].vertex = (uint16*) malloc(sphere->polygon[0].vertexLength * sizeof(uint16));

	q3dPolyhedronCompile(sphere);

	// Establish constants used in sphere generation
	float fDeltaRingAngle = ( F_PI / nRings );
	float fDeltaSegAngle = ( 2.0f * F_PI / nSegments );

	uint16 wVerticeIndex = 0 ;
	uint16 pos = 0;
	uint16 pos2 = 0;
	// Generate the group of rings for the sphere
	for( int ring = 0; ring < nRings + 1 ; ring++ )
	{
		float r0 = fsin( ring * fDeltaRingAngle );
		float y0 = fcos( ring * fDeltaRingAngle );


		if ( ring != nRings ) {
		}

		// Generate the group of segments for the current ring
		for( int seg = 0; seg < nSegments + 1 ; seg++ )
		{
			float x0 = r0 * fsin( seg * fDeltaSegAngle );
			float z0 = r0 * fcos( seg * fDeltaSegAngle );

			// Add one vertices to the strip which makes up the sphere
			q3dVertexSet3f(&sphere->vertex[pos], x0, -y0, z0);
			q3dVectorSetV(&sphere->_uVertexNormal[pos], &sphere->vertex[pos]);
//			q3dVectorNormalize(&sphere->_uVertexNormal[pos]);
			pos++;
//			pVertex->n = pVertex->p ; 
//			pVertex->color = 0xffffffff ; 
//			pVertex->tu = (float) seg / (float) nSegments;
//			pVertex->tv = (float) ring / (float) nRings;


			// add two indices except for last ring 
			if ( ring != nRings ) 
			{
				sphere->polygon[0].vertex[pos2] = wVerticeIndex ;
				pos2++;
				sphere->polygon[0].vertex[pos2] = wVerticeIndex + ( nSegments + 1 ) ; 
				pos2++;
				wVerticeIndex ++ ; 
			}

		} // end for seg 

	} // end for ring
	return sphere;
}
q3dTypePolyhedron *sphere;
q3dTypeFiller fillerPlayers;
Game::Game() {
	q3dMatrixInit();

	sphere = generateSphere();
	q3dFillerCellInit(&fillerPlayers);
	q3dCameraInit(&cam);
	cam._pos.y = -5;
	cam._pos.z = 5;

	for (int i = 0; i < 4; i++)
		player[i].setController(i);
}

Game::~Game() {
	q3dPolyhedronFree(sphere);
	free(sphere);
}

void Game::update() {
	for (int i = 0; i < 4; i++)
		player[i].update();

	level.update();
}

void Game::draw() {
	// begin render with TA
	pvr_scene_begin();
	pvr_list_begin(PVR_LIST_OP_POLY);

	// todo.. draw..
	for (int i = 0; i < 4; i++) {
		// TODO: camera stuff..
		player[i].draw(&cam);
	}

	pvr_list_finish();

	pvr_scene_finish();
	pvr_wait_ready();
}

