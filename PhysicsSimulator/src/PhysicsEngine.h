#pragma once
#include "Core.h"
#include "includes/Vector.h"
#include "Universe.h"
#include <string>

float DistanceDifference(Vector2* point, Vector2* point_two);
static Vector2 SDLPointToVec2(SDL_Point* point);

class PhysicsEngine {	
public:
	void UpdatePhysics(PhysicsObject* first) const;
};