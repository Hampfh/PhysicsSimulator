#pragma once
#include "Core.h"
#include "includes/Vector.h"
#include "Universe.h"
#include <string>
#include <cmath>

// 500 first characters of PI
#define PI M_PI

class PhysicsEngine {	
public:
	int CollisionManagement(PhysicsObject* first, PhysicsObject* second, Universe* universe) const;
	void UpdatePhysics(Universe* universe, float delta_time, float simulation_speed) const;

	static double CalculateForceBetweenObjects(Vector2* pos1, Vector2* pos2, double m1, double m2);
	static double CalculateForceBetweenObjects(Vector2 pos1, Vector2 pos2, double m1, double m2);
	static double DistanceDifference(PhysicsObject* point, PhysicsObject* point_two);
	static double DistanceDifference(Vector2* point, Vector2* point_two);
	static void ApplyIndividualForce(PhysicsObject* object, Vector2 target_position, float amplified_force);
};