#include "primitives.h"

q3dTypePolyhedron *generateSphere() {
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
			q3dVertexSet3f(&sphere->vertex[pos], x0, -y0, -z0);
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

q3dTypePolyhedron *generateCube(float size) {
	q3dTypePolyhedron *cube = (q3dTypePolyhedron*) malloc(sizeof(q3dTypePolyhedron));
	q3dPolyhedronInit(cube);

	cube->vertexLength = 8;
	cube->vertex = (q3dTypeVertex*) malloc(8 * sizeof(q3dTypeVertex));
	q3dVertexSet3f(&cube->vertex[0], -size, -size, -size);
	q3dVertexSet3f(&cube->vertex[1], -size,  size, -size);
	q3dVertexSet3f(&cube->vertex[2],  size,  size, -size);
	q3dVertexSet3f(&cube->vertex[3],  size, -size, -size);
	q3dVertexSet3f(&cube->vertex[4],  size, -size,  size);
	q3dVertexSet3f(&cube->vertex[5],  size,  size,  size);
	q3dVertexSet3f(&cube->vertex[6], -size,  size,  size);
	q3dVertexSet3f(&cube->vertex[7], -size, -size,  size);

	cube->polygonLength = 6;
	cube->polygon = (q3dTypePolygon*) malloc(6 * sizeof(q3dTypePolyhedron));

	// front
	q3dPolygonInit(&cube->polygon[0]);
	cube->polygon[0].vertexLength = 4;
	cube->polygon[0].vertex = (uint16*) malloc(4 * sizeof(int));
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
	cube->polygon[1].vertex = (uint16*) malloc(4 * sizeof(int));
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
	cube->polygon[2].vertex = (uint16*) malloc(4 * sizeof(int));;
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
	cube->polygon[3].vertex = (uint16*) malloc(4 * sizeof(int));
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
	cube->polygon[4].vertex = (uint16*) malloc(4 * sizeof(int));;
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
	cube->polygon[5].vertex = (uint16*) malloc(4 * sizeof(int));;
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
	q3dColorSet3f(&cube->material.color , size, size, size);
//	cube->vertexmode = Q3D_TRANSLUCENT | Q3D_TEXTURED;
	return cube;
}

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
			cube->polygon[pos].vertex = (uint16*) malloc(4 * sizeof(uint16));;
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


