#pragma once
#include "Core.h"
#include "includes/Vector.h"
#include "Universe.h"
#include <string>
#include <cmath>

// 500 first characters of PI
#define PI 3.14159265358979323846264338327950288419716939937510582097494459230781640628620899862803482534211706798214808651328230664709384460955058223172535940812848111745028410270193852110555964462294895493038196442881097566593344612847564823378678316527120190914564856692346034861045432664821339360726024914127372458700660631558817488152092096282925409171536436789259036001133053054882046652138414695194151160943305727036575959195309218611738193261179310511854807446237996274956735188575272489122793818301194912

double CalculateForceBetweenObjects();
double DistanceDifference(PhysicsObject* point, PhysicsObject* point_two);
double DistanceDifference(Vector2* point, Vector2* point_two);
void ApplyIndividualForce(PhysicsObject* object, Vector2 target_position, float amplified_force);

class PhysicsEngine {	
public:
	int CollisionManagement(PhysicsObject* first, PhysicsObject* second, Universe* universe) const;
	void UpdatePhysics(Universe* universe, float delta_time, float simulation_speed) const;
};