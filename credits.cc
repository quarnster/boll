#include <math.h>
#include "credits.h"

#include "lib/Point3D.h"
#include "lib/Keyframer.h"
#include "lib/primitives.h"

#include <plx/font.h>
#include <plx/list.h>
#include <plx/dr.h>





#define VECTOR Point3D

class PLANE {
public:
	float equation[4];
	float distance;
	VECTOR origin;
	VECTOR normal;

	PLANE::PLANE();
	PLANE(const VECTOR& origin, const VECTOR& normal);
	PLANE(const VECTOR& p1, const VECTOR& p2, const VECTOR& p3);
	bool isFrontFacingTo(const VECTOR& direction) const;
	double signedDistanceTo(const VECTOR& point) const;
};

class CollisionPacket {
public:
//	VECTOR eRadius; // ellipsoid radius
	// Information about the move being requested: (in R3)
//	VECTOR R3Velocity;
//	VECTOR R3Position;


	// Information about the move being requested: (in eSpace)
	VECTOR velocity;
	VECTOR normalizedVelocity;
	VECTOR basePoint;

	// Hit information
	bool foundCollision;
	double nearestDistance;
	double t;
	VECTOR intersectionPoint;
	PLANE plane;
};

PLANE::PLANE() {
}
PLANE::PLANE(const VECTOR& origin, const VECTOR& normal) {
	this->normal = normal;
	this->origin = origin;
	equation[0] = normal.x;
	equation[1] = normal.y;
	equation[2] = normal.z;
	distance = equation[3] = -(normal.x*origin.x+normal.y*origin.y +normal.z*origin.z);
}

// Construct from triangle:
PLANE::PLANE(const VECTOR& p1,const VECTOR& p2, const VECTOR& p3) {
	normal = (p2-p1).cross(p3-p1);
	normal.normalize();
	origin = p1;
	equation[0] = normal.x;
	equation[1] = normal.y;
	equation[2] = normal.z;
	equation[3] = -(normal.x*origin.x+normal.y*origin.y +normal.z*origin.z);
}

bool PLANE::isFrontFacingTo(const VECTOR& direction) const {
	double dot = normal.dot(direction);
	return (dot >= 0);
}
double PLANE::signedDistanceTo(const VECTOR& point) const {
	return (point.dot(normal)) + equation[3];
}

// typedef unsigned int uint32;

#define in(a) ((uint32&) a)

bool checkPointInTriangle(const VECTOR& point, const VECTOR& pa,const VECTOR& pb, const VECTOR& pc) {
	VECTOR e10=pb-pa;
	VECTOR e20=pc-pa;

	float a = e10.dot(e10);
	float b = e10.dot(e20);
	float c = e20.dot(e20);
	float ac_bb=(a*c)-(b*b);
	VECTOR vp(point.x-pa.x, point.y-pa.y, point.z-pa.z);

	float d = vp.dot(e10);
	float e = vp.dot(e20);
	float x = (d*c)-(e*b);
	float y = (e*a)-(d*b);
	float z = x+y-ac_bb;

	return (( in(z)& ~(in(x)|in(y)) ) & 0x80000000);
}

Point3D operator*(const float& mul, const Point3D& p) {
	Point3D tmp;
	tmp.x = mul * p.x;
	tmp.y = mul * p.y;
	tmp.z = mul * p.z;

	return tmp;
}
bool getLowestRoot(float a, float b, float c, float maxR, float* root) {
	// Check if a solution exists
	float determinant = b*b - 4.0f*a*c;
	// If determinant is negative it means no solutions.
	if (determinant < 0.0f)
		return false;

	// calculate the two roots: (if determinant == 0 then
	// x1==x2 but let s disregard that slight optimization)
	float sqrtD = fsqrt(determinant);
	float r1 = (-b - sqrtD) / (2*a);
	float r2 = (-b + sqrtD) / (2*a);

	// Sort so x1 <= x2
	if (r1 > r2) {
		float temp = r2;
		r2 = r1;
		r1 = temp;
	}

	// Get lowest root:
	if (r1 > 0 && r1 < maxR) {
		*root = r1;
		return true;
	}
	// It is possible that we want x2 - this can happen
	// if x1 < 0
	if (r2 > 0 && r2 < maxR) {
		*root = r2;
		return true;
	}

	// No (valid) solutions
	return false;
}

// Assumes: p1,p2 and p3 are given in ellisoid space:
void checkTriangle(CollisionPacket* colPackage, const VECTOR& p1,const VECTOR& p2,const VECTOR& p3) {
	// Make the plane containing this triangle.
	PLANE trianglePlane(p1,p2,p3);

	// Is triangle front-facing to the velocity vector?
	// We only check front-facing triangles
	// (your choice of course)
//	if (trianglePlane.isFrontFacingTo( colPackage->normalizedVelocity)) {
		// Get interval of plane intersection:
		double t0, t1;
		bool embeddedInPlane = false;

		// Calculate the signed distance from sphere
		// position to triangle plane
		double signedDistToTrianglePlane = trianglePlane.signedDistanceTo(colPackage->basePoint);
		// cache this as we re going to use it a few times below:
		float normalDotVelocity = trianglePlane.normal.dot(colPackage->velocity);
		// if sphere is travelling parrallel to the plane:
		if (normalDotVelocity == 0.0f) {
			if (fabs(signedDistToTrianglePlane) >= 1.0f) {
				// Sphere is not embedded in plane.
				// No collision possible:
				return;
			} else {
				// sphere is embedded in plane.
				// It intersects in the whole range [0..1]
				embeddedInPlane = true;
				t0 = 0.0; t1 = 1.0;
			}
		} else {
			// N dot D is not 0. Calculate intersection interval:
			t0 = (-1.0 - signedDistToTrianglePlane) / normalDotVelocity;
			t1 = ( 1.0 - signedDistToTrianglePlane) / normalDotVelocity;

			// Swap so t0 < t1
			if (t0 > t1) {
				double temp = t1;
				t1 = t0;
				t0 = temp;
			}

			// Check that at least one result is within range:
			if (t0 > 1.0f || t1 < 0.0f) {
				// Both t values are outside values [0,1]
				// No collision possible:
				return;
			}

			// Clamp to [0,1]
			if (t0 < 0.0) t0 = 0.0;
			if (t1 < 0.0) t1 = 0.0;
			if (t0 > 1.0) t0 = 1.0;
			if (t1 > 1.0) t1 = 1.0;
		}

		// OK, at this point we have two time values t0 and t1
		// between which the swept sphere intersects with the
		// triangle plane. If any collision is to occur it must
		// happen within this interval.
		VECTOR collisionPoint;
		bool foundCollison = false;
		float t = 1.0;

		// First we check for the easy case - collision inside
		// the triangle. If this happens it must be at time t0
		// as this is when the sphere rests on the front side
		// of the triangle plane. Note, this can only happen if
		// the sphere is not embedded in the triangle plane.
		if (!embeddedInPlane) {
			VECTOR planeIntersectionPoint = (colPackage->basePoint-trianglePlane.normal) + t0*colPackage->velocity;
			if (checkPointInTriangle(planeIntersectionPoint, p1,p2,p3)) {
				foundCollison = true;
				t = t0;
				collisionPoint = planeIntersectionPoint;
			}
		}

		// if we haven t found a collision already we ll have to
		// sweep sphere against points and edges of the triangle.
		// Note: A collision inside the triangle (the check above)
		// will always happen before a vertex or edge collision!
		// This is why we can skip the swept test if the above
		// gives a collision!
		if (foundCollison == false) {
			// some commonly used terms:
			VECTOR velocity = colPackage->velocity;
			VECTOR base = colPackage->basePoint;
			float velocitySquaredLength = velocity.squaredLength();
			float a,b,c; // Params for equation
			float newT;

			// For each vertex or edge a quadratic equation have to
			// be solved. We parameterize this equation as
			// a*t^2 + b*t + c = 0 and below we calculate the
			// parameters a,b and c for each test.
			// Check against points:
			a = velocitySquaredLength;

			// P1
			b = 2.0*(velocity.dot(base-p1));
			c = (p1-base).squaredLength() - 1.0;
			if (getLowestRoot(a,b,c, t, &newT)) {
				t = newT;
				foundCollison = true;
				collisionPoint = p1;
			}
			
			// P2
			b = 2.0*(velocity.dot(base-p2));
			c = (p2-base).squaredLength() - 1.0;
			if (getLowestRoot(a,b,c, t, &newT)) {
				t = newT;
				foundCollison = true;
				collisionPoint = p2;
			}

			// P3
			b = 2.0*(velocity.dot(base-p3));
			c = (p3-base).squaredLength() - 1.0;
			if (getLowestRoot(a,b,c, t, &newT)) {
				t = newT;
				foundCollison = true;
				collisionPoint = p3;
			}

			// Check agains edges:

			// p1 -> p2:
			VECTOR edge = p2-p1;
			VECTOR baseToVertex = p1 - base;
			float edgeSquaredLength = edge.squaredLength();
			float edgeDotVelocity = edge.dot(velocity);
			float edgeDotBaseToVertex = edge.dot(baseToVertex);

			// Calculate parameters for equation
			a = edgeSquaredLength*-velocitySquaredLength + edgeDotVelocity*edgeDotVelocity;
			b = edgeSquaredLength*(2*velocity.dot(baseToVertex))- 2.0*edgeDotVelocity*edgeDotBaseToVertex;
			c = edgeSquaredLength*(1-baseToVertex.squaredLength())+ edgeDotBaseToVertex*edgeDotBaseToVertex;

			// Does the swept sphere collide against infinite edge?
			if (getLowestRoot(a,b,c, t, &newT)) {
				// Check if intersection is within line segment:
				float f = (edgeDotVelocity*newT-edgeDotBaseToVertex)/ edgeSquaredLength;
				if (f >= 0.0 && f <= 1.0) {
					// intersection took place within segment.
					t = newT;
					foundCollison = true;
					collisionPoint = p1 + f*edge;
				}
			}

			// p2 -> p3:
			edge = p3-p2;
			baseToVertex = p2 - base;
			edgeSquaredLength = edge.squaredLength();
			edgeDotVelocity = edge.dot(velocity);
			edgeDotBaseToVertex = edge.dot(baseToVertex);
			a = edgeSquaredLength*-velocitySquaredLength + edgeDotVelocity*edgeDotVelocity;
			b = edgeSquaredLength*(2*velocity.dot(baseToVertex))- 2.0*edgeDotVelocity*edgeDotBaseToVertex;
			c = edgeSquaredLength*(1-baseToVertex.squaredLength())+ edgeDotBaseToVertex*edgeDotBaseToVertex;
			if (getLowestRoot(a,b,c, t, &newT)) {
				float f = (edgeDotVelocity*newT-edgeDotBaseToVertex)/ edgeSquaredLength;
				if (f >= 0.0 && f <= 1.0) {
					t = newT;
					foundCollison = true;
					collisionPoint = p2 + f*edge;
				}
			}

			// p3 -> p1:
			edge = p1-p3;
			baseToVertex = p3 - base;
			edgeSquaredLength = edge.squaredLength();
			edgeDotVelocity = edge.dot(velocity);
			edgeDotBaseToVertex = edge.dot(baseToVertex);
			a = edgeSquaredLength*-velocitySquaredLength + edgeDotVelocity*edgeDotVelocity;
			b = edgeSquaredLength*(2*velocity.dot(baseToVertex))- 2.0*edgeDotVelocity*edgeDotBaseToVertex;
			c = edgeSquaredLength*(1-baseToVertex.squaredLength())+ edgeDotBaseToVertex*edgeDotBaseToVertex;
			if (getLowestRoot(a,b,c, t, &newT)) {
				float f = (edgeDotVelocity*newT-edgeDotBaseToVertex)/ edgeSquaredLength;
				if (f >= 0.0 && f <= 1.0) {
					t = newT; foundCollison = true;
					collisionPoint = p3 + f*edge;
				}
			}
		}
		// Set result:
		if (foundCollison == true) {
			// distance to collision:  t  is time of collision
			float distToCollision = t*colPackage->velocity.length();
			// Does this triangle qualify for the closest hit?
			// it does if it s the first hit or the closest
			if (colPackage->foundCollision == false || distToCollision < colPackage->nearestDistance) {
				// Collision information nessesary for sliding
				colPackage->nearestDistance = distToCollision;
				colPackage->intersectionPoint = collisionPoint;
				colPackage->foundCollision = true;
				colPackage->plane.normal = trianglePlane.normal;
				colPackage->plane.equation[3] = trianglePlane.equation[3];
				colPackage->t = t;
			}
		}
//	} // if not backface
}
/*
void CharacterEntity::collideAndSlide(const VECTOR& vel, const VECTOR& gravity) {
	// Do collision detection:
	collisionPackage->R3Position = position;
	collisionPackage->R3Velocity = vel;

	// calculate position and velocity in eSpace
	VECTOR eSpacePosition = collisionPackage->R3Position/ collisionPackage->eRadius;
	VECTOR eSpaceVelocity = collisionPackage->R3Velocity/ collisionPackage->eRadius;

	// Iterate until we have our final position.
	collisionRecursionDepth = 0;
	VECTOR finalPosition = collideWithWorld(eSpacePosition, eSpaceVelocity);

	// Add gravity pull:
	// To remove gravity uncomment from here .....

	// Set the new R3 position (convert back from eSpace to R3
	collisionPackage->R3Position = finalPosition*collisionPackage->eRadius;
	collisionPackage->R3Velocity = gravity;
	eSpaceVelocity = gravity/collisionPackage->eRadius;
	collisionRecursionDepth = 0;
	finalPosition = collideWithWorld(finalPosition, eSpaceVelocity);

	// ... to here
	// Convert final result back to R3:
	finalPosition = finalPosition*collisionPackage->eRadius;

	// Move the entity (application specific function)
	MoveTo(finalPosition);
}

// Set this to match application scale..
const float unitsPerMeter = 100.0f;
VECTOR CharacterEntity::collideWithWorld(const VECTOR& pos, const VECTOR& vel) {
	// All hard-coded distances in this function is
	// scaled to fit the setting above..
	float unitScale = unitsPerMeter / 100.0f;
	float veryCloseDistance = 0.005f * unitScale;

	// do we need to worry?
	if (collisionRecursionDepth>5)
		return pos;

	// Ok, we need to worry:
	collisionPackage->velocity = vel;
	collisionPackage->normalizedVelocity = vel;
	collisionPackage->normalizedVelocity.normalize();
	collisionPackage->basePoint = pos;
	collisionPackage->foundCollision = false;

	// Check for collision (calls the collision routines)
	// Application specific!!
	world->checkCollision(collisionPackage);

	// If no collision we just move along the velocity
	if (collisionPackage->foundCollision == false) {
		return pos + vel;
	}
	// *** Collision occured ***
	// The original destination point
	VECTOR destinationPoint = pos + vel;
	VECTOR newBasePoint = pos;

	// only update if we are not already very close
	// and if so we only move very close to intersection..not
	// to the exact spot.
	if (collisionPackage->nearestDistance>=veryCloseDistance) {
		VECTOR V = vel;
		V.SetLength(collisionPackage->nearestDistanceveryCloseDistance);
		newBasePoint = collisionPackage->basePoint + V;

		// Adjust polygon intersection point (so sliding
		// plane will be unaffected by the fact that we
		// move slightly less than collision tells us)
		V.normalize();
		collisionPackage->intersectionPoint -= veryCloseDistance * V;
	}

	// Determine the sliding plane
	VECTOR slidePlaneOrigin = collisionPackage->intersectionPoint;
	VECTOR slidePlaneNormal = newBasePoint-collisionPackage->intersectionPoint;
	slidePlaneNormal.normalize();

	PLANE slidingPlane(slidePlaneOrigin,slidePlaneNormal);

	// Again, sorry about formatting.. but look carefully ;)
	VECTOR newDestinationPoint = destinationPoint - slidingPlane.signedDistanceTo(destinationPoint)* slidePlaneNormal;

	// Generate the slide vector, which will become our new
	// velocity vector for the next iteration
	VECTOR newVelocityVector = newDestinationPoint - collisionPackage->intersectionPoint;

	// Recurse:
	// dont recurse if the new velocity is very small
	if (newVelocityVector.length() < veryCloseDistance) {
		return newBasePoint;
	}
	collisionRecursionDepth++;
	return collideWithWorld(newBasePoint,newVelocityVector);
}
*/
static plx_font_t *fnt;
static plx_fcxt_t *fcxt;

#define LINENUM 32
static int width[LINENUM];

static float fontSize = 16;
static char cred_text[LINENUM][45] = {
	"== Programming ==",
	"Fredrik \"quarn\" Ehnbom",
	"",
	"== Music and sfx ==",
	"Andreas \"zantac\" Hedlund",
	"",
	"This has been the result of",
	"around 50 days of work. The project",
	"started at January 23rd and was submitted",
	"to the dream-on competition at March 16th",
	"(one day before the deadline..)",
	"",
	"This game would not have been possible",
	"without all the people writing code",
	"(and the very few who write docs..)",
	"for the dreamcast video gaming system.",
	"Therefore I would like to thank the",
	"following people (in no special order):",
	"",
	"Marcus Comstedt",
	"Mikael Kalms",
	"Dan Potter",
	"Lars Olsson",
	"Yamoto",
	"",
	"Of course, this list could be a lot",
	"longer, but these are the names that",
	"came to my mind",
	
	
};

static uint64 startTime = 0;

static KeyFramer kFrame;
static KeyFrame frames[4];

static q3dTypeVertex world_coordinates[8];

Credits::Credits() {
	q3dMatrixInit();
	// load debug-font
	fnt = plx_font_load("font.txf");
	fcxt = plx_fcxt_create(fnt, PVR_LIST_TR_POLY);
	plx_fcxt_setcolor4f(fcxt, 1, 1, 1, 1);
	plx_fcxt_setsize(fcxt, fontSize);
	printf("fnt: %d, fcxt: %d\n", fnt, fcxt);

	for (int i = 0; i < LINENUM; i++) {
		float l, u, d, r;
		plx_fcxt_str_metrics(fcxt, cred_text[i], &l, &u, &r, &d);
		width[i] = 640 / 2 - (int) ((l+r) / 2);
	}
	frames[0].continuity = frames[3].continuity = 1.0f;
	frames[1].continuity = frames[2].continuity = 0.0f;
	frames[1].bias = frames[2].bias = 1.0f;
	frames[0].set(0, 480+fontSize, 0); frames[0].time = 0;
	frames[1].set(0.8, 320+fontSize, 0); frames[1].time = 0.5;
	frames[2].set(0.8, 160+fontSize, 0); frames[2].time = 8.0;
	frames[3].set(0, 0-fontSize, 0); frames[3].time = 8.5;
	kFrame.generate(frames, 4);

	sphere = generateSphere();
	q3dPolyhedronCompile(sphere);

	cube = generateCube(10);
	q3dPolyhedronCompile(cube);

	q3dFillerStandardInit(&sphereFiller);
	sphere->material.header = sphereFiller.defaultHeader;

	q3dFillerStandardInit(&cubeFiller);
	pvr_poly_cxt_col(&cubeFiller.defaultCxt, PVR_LIST_TR_POLY);
	cubeFiller.defaultCxt.gen.culling = PVR_CULLING_NONE;
	pvr_poly_compile(&cubeFiller.defaultHeader, &cubeFiller.defaultCxt);
	cube->material.header = cubeFiller.defaultHeader;

	q3dCameraInit(&cam);
	cam._pos.z = -20;
/*
	for (int i = 0; i < 6; i++) {
		printf("normal[%d]: (%.2f, %.2f, %.2f)\n", i,
			cube->_uPolygonNormal[i].x,
			cube->_uPolygonNormal[i].y,
			cube->_uPolygonNormal[i].z
		);
	}
*/
}

Credits::~Credits() {
	plx_fcxt_destroy(fcxt);
	plx_font_destroy(fnt);

	printf("polyhedron free cube: ...");
	q3dPolyhedronFree(cube);
	printf("done!\nfree cube: ...");
	free(cube);
	printf("done!\n");

	printf("polyhedronfree sphere: ..."); 
	q3dPolyhedronFree(sphere);
	printf("done!\nfree sphere: ...");
	free(sphere);
	printf("done!\n");
}

static Point3D velocity;
void Credits::draw() {
	static Point3D key;
	static CollisionPacket packet;
	static Point3D p1;
	static Point3D p2;
	static Point3D p3;

	q3dMatrixIdentity();

	q3dMatrixRotateX(cube->_agl.x);
	q3dMatrixRotateY(cube->_agl.y);
	q3dMatrixRotateZ(cube->_agl.z);
	q3dMatrixTransform(cube->vertex, world_coordinates, 8, sizeof(q3dTypeVertex));

	if (startTime == 0) startTime = timer_ms_gettime64();

	float time = (timer_ms_gettime64() - startTime) / 1000.0f;

	maple_device_t *dev = maple_enum_dev(0, 0);
	if (dev != NULL && dev->info.functions & MAPLE_FUNC_CONTROLLER) {
		cont_state_t* s = (cont_state_t*) maple_dev_status(dev);

		cube->_agl.x += s->joyy / 4096.0f;
		cube->_agl.y += s->joyx / 4096.0f;

		if (s->buttons & CONT_A) {
			sphere->_pos.x = sphere->_pos.y = sphere->_pos.z = 0;
			velocity.set(0,0,0);
//			velocity.x = velocity.y = velocity.z = 0;
		}
	}
	packet.foundCollision = false;
	packet.nearestDistance = 10;
	packet.basePoint.set(sphere->_pos.x, sphere->_pos.y, sphere->_pos.z);
	packet.velocity.set(velocity.x,velocity.y, velocity.z);
	packet.normalizedVelocity.set(velocity.x,velocity.y, velocity.z);
	packet.normalizedVelocity.normalize();

	for (int i = 0; i < 6; i++) {
		q3dTypeVertex *v = &world_coordinates[cube->polygon[i].vertex[0]];
		p1.set(v->x, v->y, v->z);
		v = &world_coordinates[cube->polygon[i].vertex[1]];
		p2.set(v->x, v->y, v->z);
		v = &world_coordinates[cube->polygon[i].vertex[2]];
		p3.set(v->x, v->y, v->z);
		checkTriangle(&packet, p1,p2, p3);

		v = &world_coordinates[cube->polygon[i].vertex[2]];
		p1.set(v->x, v->y, v->z);
		v = &world_coordinates[cube->polygon[i].vertex[1]];
		p2.set(v->x, v->y, v->z);
		v = &world_coordinates[cube->polygon[i].vertex[3]];
		p3.set(v->x, v->y, v->z);
		checkTriangle(&packet, p1,p2, p3);
	}

	if (packet.nearestDistance == 10) {
		// no collision
		sphere->_pos.x += packet.velocity.x;
		sphere->_pos.y += packet.velocity.y;
		sphere->_pos.z += packet.velocity.z;
	} else {
		float dist = packet.plane.signedDistanceTo(packet.basePoint);
		float t = packet.t - 0.1;
		if (t > 0.1) {
			sphere->_pos.x += t * packet.velocity.x;
			sphere->_pos.y += t * packet.velocity.y;
			sphere->_pos.z += t * packet.velocity.z;
		} else if (fabs(dist) < 1.0) {
			// we are inside of the plane.. need to move outside
			float d = -0.1 - dist;
			sphere->_pos.x -= d * packet.plane.normal.x;
			sphere->_pos.y -= d * packet.plane.normal.y;
			sphere->_pos.z -= d * packet.plane.normal.z;
		}
/*
		Point3D normal = packet.plane.normal;
		Point3D reflection = velocity - 2*(velocity.dot(normal)) * normal;
		velocity = reflection *0.75;
*/
		velocity *= 0;
		if (velocity.squaredLength() < 0.5) {
			// slide instead of bounce
		}
	}
	velocity.y -= 0.05;

//	cube->_agl.x = time;
//	cube->_agl.y = time*1.1;
//	cube->_agl.z = time*1.2;


	// begin rendering
	pvr_wait_ready();
	pvr_scene_begin();

	pvr_list_begin(PVR_LIST_OP_POLY);

	q3dColorSet3f(&sphere->material.color, 1.0f, 0.0f, 0.0f);
	q3dPolyhedronPaint(sphere, &cam, &sphereFiller);

	pvr_list_finish();

	pvr_list_begin(PVR_LIST_TR_POLY);

	// draw the cube
	q3dColorSet4f(&cube->material.color, 1.0f, 1.0f, 1.0f, 0.5);
	q3dPolyhedronPaint(cube, &cam, &cubeFiller);

	// draw credits text
	plx_fcxt_begin(fcxt);

	for (int i = 0; i < LINENUM; i++) {
		kFrame.getKey(frames, 4, &key, time - i*0.75);
		float y = key.y; //(int) (i * fontSize + 320 - time * 34);
		if (y < 0) continue;
		if (y > 480) break;

		plx_fcxt_setcolor4f(fcxt, key.x, 1, 1, 1);
		plx_fcxt_setpos(fcxt, width[i], y, 2);
		plx_fcxt_draw(fcxt, cred_text[i]);
	}

	plx_fcxt_end(fcxt);

	pvr_list_finish();

	pvr_scene_finish();
}
