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
q3dTypeFiller fillerLevel;

q3dTypeMatrix screen_matrix __attribute__((aligned(32))) = {
    { 480/4.0f, 0.0f, 0.0f, 0.0f },
    { 0.0f, 480 / 4.0f, 0.0f, 0.0f },
    { 0.0f, 0.0f, 1.0f, 0.0f },
    { 0.0f, 0.0f, 0.0f, 1.0f }
};

Game::Game() {
	q3dMatrixInit();

	sphere = generateSphere();
	q3dFillerCellInit(&fillerPlayers);
	fillerPlayers.defaultCxt.gen.clip_mode = PVR_USERCLIP_INSIDE;
	pvr_poly_compile(&fillerPlayers.defaultHeader, &fillerPlayers.defaultCxt);

	q3dFillerStandardInit(&fillerLevel);
	fillerLevel.defaultCxt.gen.clip_mode = PVR_USERCLIP_DISABLE;
	pvr_poly_compile(&fillerLevel.defaultHeader, &fillerLevel.defaultCxt);

	q3dCameraInit(&cam);
	cam._pos.y = -5;
	cam._pos.z = 5;

	for (int i = 0; i < 4; i++)
		player[i].setController(i);


	pvr_poly_cxt_t cxt;
	pvr_poly_cxt_col(
		&cxt,
		PVR_LIST_OP_POLY
	);

	pvr_poly_compile(&crossHeader, &cxt);
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

static pvr_poly_hdr_t user_clip = {
        PVR_CMD_USERCLIP, 0x00000000, 0x00000000, 0x00000000,
        0x00000000, 0x00000000, 0x00000000, 0x00000000
};
extern matrix_t projection_matrix;
void Game::draw() {

	// begin render with TA
	pvr_wait_ready();
	pvr_scene_begin();
	pvr_list_begin(PVR_LIST_OP_POLY);

	// draw..
	for (int i = 0; i < 4; i++) {
		q3dMatrixIdentity();
		q3dMatrixTranslate(0,2,-10);
		q3dMatrixRotateY(player[i].rotation.y);
		q3dMatrixTranslate(-player[i].position.x,0 /*player[i].position.y*/,-player[i].position.z);

		q3dMatrixStore(&_q3dMatrixCamera);
		if (i == 0) {
			// upper left
			screen_matrix[3][0] = 1 * 640 / 4.0f;
			screen_matrix[3][1] = 1 * 480 / 4.0f;
			user_clip.d1 = 0;
			user_clip.d2 = 0;
			user_clip.d3 = (640/2)/32-1;
			user_clip.d4 = (480/2)/32-1;
		} else if (i == 1) {
			// upper right
			screen_matrix[3][0] = 640 * 3 / 4.0f;
			screen_matrix[3][1] = 480 * 1 / 4.0f;
			user_clip.d1 = (640/2)/32;
			user_clip.d2 = 0;
			user_clip.d3 = (4*640/4)/32-1;
			user_clip.d4 = (480/2)/32-1;
		} else if (i == 2) {
			// lower left
			screen_matrix[3][0] = 1 * 640 / 4.0f;
			screen_matrix[3][1] = 3 * 480 / 4.0f;
			user_clip.d1 = 0;
			user_clip.d2 = (480/2)/32;
			user_clip.d3 = (640/2)/32-1;
			user_clip.d4 = (4*480/4)/32-2;
		} else {
			// lower right
			screen_matrix[3][0] = 3 * 640 / 4.0f;
			screen_matrix[3][1] = 3 * 480 / 4.0f;
			user_clip.d1 = (640/2)/32;
			user_clip.d2 = (480/2)/32;
			user_clip.d3 = (4*640/4)/32-1;
			user_clip.d4 = (4*480/4)/32-2;
		}
		pvr_prim(&user_clip, sizeof(pvr_poly_hdr_t));
		q3dMatrixLoad(&screen_matrix);
		q3dMatrixApply(&projection_matrix);
		q3dMatrixStore(&_q3dMatrixPerspective);

		for (int j = 0; j < 4; j++) {
			// TODO: camera stuff..
			player[j].draw();
		}
		if (i == 1) {
			level.draw();
		}
	}

	// commit cross
	pvr_prim(&crossHeader, sizeof(pvr_poly_hdr_t));
	pvr_vertex_t vert;
	vert.argb = PVR_PACK_COLOR(1.0f, 0.0f, 0.0f, 0.0f);
	vert.z = 1 / 0.01f;

	// vertical line
	vert.flags = PVR_CMD_VERTEX;
	vert.x = 640/2-1; vert.y = 480-32;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	/*vert.x = 640/2-1;*/ vert.y = 0;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	vert.x = 640/2+1; vert.y = 480-32;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	vert.flags = PVR_CMD_VERTEX_EOL;
	/*vert.x = 448+xadd;*/ vert.y = 0;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	// horizontal line
	vert.flags = PVR_CMD_VERTEX;
	vert.x = 0; vert.y = (480-32)/2+1;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	/*vert.x = 640/2-1;*/ vert.y = (480-32)/2-1;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	vert.x = 640; vert.y = (480-32)/2+1;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	vert.flags = PVR_CMD_VERTEX_EOL;
	/*vert.x = 448+xadd;*/ vert.y = (480-32)/2-1;
	pvr_prim(&vert, sizeof(pvr_vertex_t));

	// todo: draw health, score, frags, etc-display

	pvr_list_finish();

	pvr_scene_finish();
}

