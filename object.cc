#include "object.h"

Object::Object() {
	q3dVertexInit(&position);
	q3dVertexInit(&direction);
}

void Object::update() {
	// Draw to screen
	// update position
	// and everything else needed each frame
}

