#include <kos.h>
#include <png/png.h>

#include <q3d.h>

q3dTypePolyhedron *generateTorus(int n, double r) {
	q3dTypePolyhedron *cube = (q3dTypePolyhedron*) malloc(sizeof(q3dTypePolyhedron));
	q3dPolyhedronInit(cube);

	int i, j, pos;
	float delta = 2 * F_PI / n;
	float alpha, cosa, sina;
	float beta, x;
	int i1, j1;
	int in;

	cube->vertexLength = n * n;
	cube->polygonLength = n * n;
	cube->vertex = (q3dTypeVertex*) malloc(cube->vertexLength*sizeof(q3dTypeVertex));
	cube->polygon = (q3dTypePolygon*) malloc(cube->polygonLength *sizeof(q3dTypePolygon));

	pos = 0;
	for (i = 0; i < n; i++) {
		alpha = i * delta;
		cosa = fcos(alpha);
		sina = fsin(alpha);

		for (j = 0; j < n; j++) {
			beta = j * delta;
			x = r + fcos(beta);

			cube->vertex[pos].x = cosa * x;
			cube->vertex[pos].y = sina * x;
			cube->vertex[pos].z = fsin(beta);
			pos++;
		}
	}

	pos = 0;
	for (i = 0; i < n; i++) {
		float it = i;
		i1 = (i + 1) % n;
		i1 *= n;
		in = i * n;
		
		for (j = 0; j < n; j++) {
			float ij = j;
			j1 = (j + 1) % n;

			q3dPolygonInit(&cube->polygon[pos]);
			cube->polygon[pos].vertexLength = 4;
			cube->polygon[pos].vertex = (uint16*) malloc(4 * sizeof(uint16));
			cube->polygon[pos].vertex[0] = in + j;
			cube->polygon[pos].vertex[1] = i1 + j;
			cube->polygon[pos].vertex[2] = in + j1;
			cube->polygon[pos].vertex[3] = i1 + j1;

			cube->polygon[pos].texel = (q3dTypeTexel*) malloc(4 * sizeof(q3dTypeTexel));
			cube->polygon[pos].texel[0].u = it/n;
			cube->polygon[pos].texel[0].v = ij/n;
			cube->polygon[pos].texel[1].u = (it+1)/n;
			cube->polygon[pos].texel[1].v = ij/n;
			cube->polygon[pos].texel[2].u = it/n;
			cube->polygon[pos].texel[2].v = (ij+1)/n;
			cube->polygon[pos].texel[3].u = (it+1)/n;
			cube->polygon[pos].texel[3].v = (ij+1)/n;

			pos++;
		}
	}
	q3dColorSet3f(&cube->material.color, 0.5f, 0.25f, 1.0f);

	return cube;
}

q3dTypePolyhedron *generateCube() {
	q3dTypePolyhedron *cube = (q3dTypePolyhedron*) malloc(sizeof(q3dTypePolyhedron));
	q3dPolyhedronInit(cube);

	cube->vertexLength = 8;
	cube->vertex = (q3dTypeVertex*) malloc(8 * sizeof(q3dTypeVertex));
	q3dVertexSet3f(&cube->vertex[0], -1.0f,  1.0f, -1.0f);
	q3dVertexSet3f(&cube->vertex[1], -1.0f, -1.0f, -1.0f);
	q3dVertexSet3f(&cube->vertex[2],  1.0f, -1.0f, -1.0f);
	q3dVertexSet3f(&cube->vertex[3],  1.0f,  1.0f, -1.0f);
	q3dVertexSet3f(&cube->vertex[4],  1.0f,  1.0f,  1.0f);
	q3dVertexSet3f(&cube->vertex[5],  1.0f, -1.0f,  1.0f);
	q3dVertexSet3f(&cube->vertex[6], -1.0f, -1.0f,  1.0f);
	q3dVertexSet3f(&cube->vertex[7], -1.0f,  1.0f,  1.0f);

	cube->polygonLength = 6;
	cube->polygon = (q3dTypePolygon*) malloc(6 * sizeof(q3dTypePolyhedron));

	// front
	q3dPolygonInit(&cube->polygon[0]);
	cube->polygon[0].vertexLength = 4;
	cube->polygon[0].vertex = (uint16*) malloc(4 * sizeof(uint16));
	cube->polygon[0].vertex[0] = 0;
	cube->polygon[0].vertex[1] = 1;
	cube->polygon[0].vertex[2] = 3;
	cube->polygon[0].vertex[3] = 2;
/*
	cube->polygon[0].texCoord = new Point2D[4];
	cube->polygon[0].texCoord[0].u = 0;
	cube->polygon[0].texCoord[0].v = 1;
	cube->polygon[0].texCoord[1].u = 0;
	cube->polygon[0].texCoord[1].v = 1;
	cube->polygon[0].texCoord[2].u = 1;
	cube->polygon[0].texCoord[2].v = 1;
	cube->polygon[0].texCoord[3].u = 1;
	cube->polygon[0].texCoord[3].v = 1;
*/
	// front
	q3dPolygonInit(&cube->polygon[1]);
	cube->polygon[1].vertexLength = 4;
	cube->polygon[1].vertex = (uint16*) malloc(4 * sizeof(uint16));
	cube->polygon[1].vertex[0] = 7;
	cube->polygon[1].vertex[1] = 6;
	cube->polygon[1].vertex[2] = 0;
	cube->polygon[1].vertex[3] = 1;
/*
	cube->polygon[1].texCoord = new Point2D[4];
	cube->polygon[1].texCoord[0].u = 0;
	cube->polygon[1].texCoord[0].v = 1;
	cube->polygon[1].texCoord[1].u = 0;
	cube->polygon[1].texCoord[1].v = 1;
	cube->polygon[1].texCoord[2].u = 1;
	cube->polygon[1].texCoord[2].v = 1;
	cube->polygon[1].texCoord[3].u = 1;
	cube->polygon[1].texCoord[3].v = 1;
*/
	// front
	q3dPolygonInit(&cube->polygon[2]);
	cube->polygon[2].vertexLength = 4;
	cube->polygon[2].vertex = (uint16*) malloc(4 * sizeof(uint16));
	cube->polygon[2].vertex[0] = 3;
	cube->polygon[2].vertex[1] = 2;
	cube->polygon[2].vertex[2] = 4;
	cube->polygon[2].vertex[3] = 5;
/*
	cube->polygon[2].texCoord = new Point2D[4];
	cube->polygon[2].texCoord[0].u = 0;
	cube->polygon[2].texCoord[0].v = 1;
	cube->polygon[2].texCoord[1].u = 0;
	cube->polygon[2].texCoord[1].v = 1;
	cube->polygon[2].texCoord[2].u = 1;
	cube->polygon[2].texCoord[2].v = 1;
	cube->polygon[2].texCoord[3].u = 1;
	cube->polygon[2].texCoord[3].v = 1;
*/
	// front
	q3dPolygonInit(&cube->polygon[3]);
	cube->polygon[3].vertexLength = 4;
	cube->polygon[3].vertex = (uint16*) malloc(4 * sizeof(uint16));
	cube->polygon[3].vertex[0] = 7;
	cube->polygon[3].vertex[1] = 0;
	cube->polygon[3].vertex[2] = 4;
	cube->polygon[3].vertex[3] = 3;
/*
	cube->polygon[3].texCoord = new Point2D[4];
	cube->polygon[3].texCoord[0].u = 0;
	cube->polygon[3].texCoord[0].v = 1;
	cube->polygon[3].texCoord[1].u = 0;
	cube->polygon[3].texCoord[1].v = 1;
	cube->polygon[3].texCoord[2].u = 1;
	cube->polygon[3].texCoord[2].v = 1;
	cube->polygon[3].texCoord[3].u = 1;
	cube->polygon[3].texCoord[3].v = 1;
*/
	// front
	q3dPolygonInit(&cube->polygon[4]);
	cube->polygon[4].vertexLength = 4;
	cube->polygon[4].vertex = (uint16*) malloc(4 * sizeof(uint16));
	cube->polygon[4].vertex[0] = 1;
	cube->polygon[4].vertex[1] = 6;
	cube->polygon[4].vertex[2] = 2;
	cube->polygon[4].vertex[3] = 5;
/*
	cube->polygon[4].texCoord = new Point2D[4];
	cube->polygon[4].texCoord[0].u = 0;
	cube->polygon[4].texCoord[0].v = 1;
	cube->polygon[4].texCoord[1].u = 0;
	cube->polygon[4].texCoord[1].v = 1;
	cube->polygon[4].texCoord[2].u = 1;
	cube->polygon[4].texCoord[2].v = 1;
	cube->polygon[4].texCoord[3].u = 1;
	cube->polygon[4].texCoord[3].v = 1;
*/
	// front
	q3dPolygonInit(&cube->polygon[5]);
	cube->polygon[5].vertexLength = 4;
	cube->polygon[5].vertex = (uint16*) malloc(4 * sizeof(uint16));
	cube->polygon[5].vertex[0] = 4;
	cube->polygon[5].vertex[1] = 5;
	cube->polygon[5].vertex[2] = 7;
	cube->polygon[5].vertex[3] = 6;
/*
	cube->polygon[5].texCoord = new Point2D[4];
	cube->polygon[5].texCoord[0].u = 0;
	cube->polygon[5].texCoord[0].v = 1;
	cube->polygon[5].texCoord[1].u = 0;
	cube->polygon[5].texCoord[1].v = 1;
	cube->polygon[5].texCoord[2].u = 1;
	cube->polygon[5].texCoord[2].v = 1;
	cube->polygon[5].texCoord[3].u = 1;
	cube->polygon[5].texCoord[3].v = 1;
*/
	q3dColorSet3f(&cube->material.color , 1.0f, 1.0f, 1.0f);
//	cube->vertexmode = Q3D_TRANSLUCENT | Q3D_TEXTURED;
	return cube;
}

uint32 qtime = 0;
void handle_time(irq_t source, irq_context_t *context) {
	int blah = timer_clear(TMU0);
	if (!blah) qtime+=2;
}

extern uint8 romdisk[];

KOS_INIT_FLAGS(INIT_DEFAULT | INIT_MALLOCSTATS);
KOS_INIT_ROMDISK(romdisk);
/*
void init_pvr() {
	pvr_init_params_t params = {
		// Enable opaque and translucent polygons with size 16 
		{ PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_16, PVR_BINSIZE_0, PVR_BINSIZE_0 },
		
		// Vertex buffer size 512K 
		512*1024
	};      

	pvr_init(&params);
}
*/
pvr_poly_cxt_t loadImage(char *name) {
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
		PVR_LIST_OP_POLY,
		PVR_TXRFMT_RGB565 | PVR_TXRFMT_TWIDDLED,
		width,
		height,
		ptr,
		PVR_FILTER_BILINEAR
	);

	return cxt;
}

int main(int argc, char **argv) {
	uint8 c;
	cont_cond_t cond;
	q3dTypePolyhedron *cube2;
	q3dTypeCamera cam;
	q3dTypeFiller fillerStandard;
	q3dTypeFiller fillerCell;
	q3dTypeFiller fillerTexture;
	q3dTypeFiller fillerEnvironment;
	q3dTypeFiller fillerWireframe;

	// Initialize KOS
	pvr_init_defaults();
	vid_set_mode(DM_640x480, PM_RGB565);

	printf("generate cube\n");
	cube2 = generateTorus(20, 2);
//	cube2 = generateCube();
//	cube2->vertexmode |= Q3D_ENVMAPPED;

	q3dMatrixInit();
	q3dFillerStandardInit(&fillerStandard);

	q3dFillerCellInit(&fillerCell);
	q3dFillerTextureInit(&fillerTexture);
	q3dFillerEnvironmentInit(&fillerEnvironment);
	q3dFillerWireframeInit(&fillerWireframe);

//	filler.drawOutline = false;
//	NormalFiller nFiller;

	fillerTexture.defaultCxt = loadImage("/rd/amiga.png");
	pvr_poly_compile(&fillerTexture.defaultHeader, &fillerTexture.defaultCxt);

	fillerEnvironment.defaultCxt = loadImage("/rd/env.png");
	pvr_poly_compile(&fillerEnvironment.defaultHeader, &fillerEnvironment.defaultCxt);

	pvr_set_bg_color(1.0f, 1.0f, 1.0f);


	printf("compile\n");
	q3dPolyhedronCompile(cube2);
	cube2->material.header = fillerStandard.defaultHeader;

//	cube2->_pos.y = 2;
//	cube2->pos.x = 2;
//	cube2->_pos.z = -20;
//	gluPerspective(90, 480.0f / 640.0f, 0.1f, 100.0f);
	cam._pos.z = 15;
//	cam.positionate();

	// Get basic stuff initialized

	timer_prime(TMU0, 500, 1);
	timer_start(TMU0);

	printf("enabled: %d\n", timer_ints_enabled(TMU0));
	printf("set handler: %d\n", irq_set_handler(TIMER_IRQ, &handle_time));
//---	c = maple_first_controller();

	qtime = 0;
	q3dTypeAngle agl;
	q3dAngleInit(&agl);

	int done = 0;
	while(!done) {
		MAPLE_FOREACH_BEGIN(MAPLE_FUNC_CONTROLLER, cont_state_t, st)
			if (st->buttons & CONT_START) {
				printf("Controller %d pressed start\n", __dev->port);
				done = true;
			}
		MAPLE_FOREACH_END();


//		cam._pos.z = fsin(qtime/500.0f)*100;
		cube2->_agl.y = -(qtime/1000.0f)*1.1f;
		cube2->_agl.x = (qtime/500.0f);

		agl.y = qtime/1000.0f;
		q3dCameraSetRotation(&cam, &agl);
//		cam._pos.z = 20 + fsin(qtime/1000.0f);

		// begin render with TA
//		pvr_wait_ready();
		pvr_scene_begin();

		pvr_list_begin(PVR_LIST_OP_POLY);

		cube2->_pos.x = fsin((1/5.0f)*2*F_PI) * 8;
		cube2->_pos.y = fcos((1/5.0f)*2*F_PI) * 8;
		cube2->material.header = fillerStandard.defaultHeader;
		q3dColorSet3f(&cube2->material.color, 0.5f, 0.25f, 1.0f);
		q3dPolyhedronPaint(cube2, &cam, &fillerStandard);

		cube2->_pos.x = fsin((2/5.0f)*2*F_PI) * 8;
		cube2->_pos.y = fcos((2/5.0f)*2*F_PI) * 8;
		cube2->material.header = fillerCell.defaultHeader;
		q3dColorSet3f(&cube2->material.color, 1.0f, 0.5f, 0.25f);
		q3dPolyhedronPaint(cube2, &cam, &fillerCell);

		cube2->_pos.x = fsin((3/5.0f)*2*F_PI) * 8;
		cube2->_pos.y = fcos((3/5.0f)*2*F_PI) * 8;
		cube2->material.header = fillerTexture.defaultHeader;
		q3dColorSet4f(&cube2->material.color, 1.0f, 1.0f, 1.0f, 1.0f);
		q3dPolyhedronPaint(cube2, &cam, &fillerTexture);

		cube2->_pos.x = fsin((4/5.0f)*2*F_PI) * 8;
		cube2->_pos.y = fcos((4/5.0f)*2*F_PI) * 8;
		cube2->material.header = fillerEnvironment.defaultHeader;
		q3dPolyhedronPaint(cube2, &cam, &fillerEnvironment);

		cube2->_pos.x = fsin((5/5.0f)*2*F_PI) * 8;
		cube2->_pos.y = fcos((5/5.0f)*2*F_PI) * 8;
		cube2->material.header = fillerStandard.defaultHeader;
		q3dPolyhedronPaint(cube2, &cam, &fillerWireframe);

		pvr_list_finish();

		pvr_scene_finish();
		pvr_wait_ready();
	}
        pvr_stats_t stat;
        pvr_get_stats(&stat);
                
        printf("framerate1: %f\n", stat.frame_rate);

	printf("pvr_mem_free\n");

	printf("free cube2\n");
	q3dPolyhedronFree(cube2);
	free(cube2);

	printf("done freeing...\n");
	timer_stop(TMU0);

	return 0;
}
