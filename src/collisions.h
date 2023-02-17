#pragma once

struct CollisionRect2D {
	float x, y, w, h;
};

struct Collision2DAABB {
	float x1, x2, y1, y2;
};

void resolveCollisions2D();