#pragma once
#include "Core.h"
#include "includes/Vector.h"
#include "Universe.h"
#include <string>

double CalculateForceBetweenObjects();
double DistanceDifference(PhysicsObject* point, PhysicsObject* point_two);
double DistanceDifference(Vector2* point, Vector2* point_two);
static Vector2 SDLPointToVec2(SDL_Point* point);
void ApplyIndividualForce(PhysicsObject* object, Vector2 target_position, float amplified_force);

class PhysicsEngine {	
public:
	void UpdatePhysics(PhysicsObject* first, float delta_time, float simulation_speed) const;
};