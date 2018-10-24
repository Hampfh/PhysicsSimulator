#include "PhysicsEngine.h"

double CalculateForceBetweenObjects(Vector2* pos1, Vector2* pos2, double m1, double m2) {	
	/* Newtons law of gravity
		  m1 * m2
	F = G --------
		    r^2
	*/
	std::cout << m2 << std::endl;

	const double G = 6.672f * pow(10, -11);
	const double r = DistanceDifference(pos1, pos2);

	return G * m1 * m2 / pow(r, 2);
}

double DistanceDifference(PhysicsObject* point, PhysicsObject* point_two) {
	const float differenceX = abs(point->GetX() - point_two->GetX());
	const float differenceY = abs(point->GetY() - point_two->GetY());

	return sqrt(pow(differenceX, 2) + pow(differenceY, 2));
}
double DistanceDifference(Vector2* point, Vector2* point_two) {
	const float differenceX = abs(point->x - point_two->x);
	const float differenceY = abs(point->y - point_two->y);

	return sqrt(pow(differenceX, 2) + pow(differenceY, 2));
}

void ApplyIndividualForce(PhysicsObject* object, Vector2 target_position, const float amplified_force) {
	Vector2* pos1 = object->GetLocation();
	Vector2 dir = target_position - *pos1;

	/* Newtons law of gravity
		  m1 * m2
	F = G --------
		    r^2
	*/
	//const float forceStrength = static_cast<float>(6.672f * pow(10, -11)) * (static_cast<float>(object->GetMass() * static_cast<float>(object->GetMass() * amplified_force)) / pow(DistanceDifference(pos1, &target_position), 2));
	const double forceStrength = CalculateForceBetweenObjects(object->GetLocation(), &target_position, object->GetMass(), object->GetMass() * DistanceDifference(pos1, &target_position) * amplified_force);
	dir = dir.SetMag(static_cast<float>(forceStrength / object->GetMass()));

	object->ApplyForce(dir);

	std::cout << DistanceDifference(pos1, &target_position) << std::endl;
}

Vector2 SDLPointToVec2(SDL_Point* point) {
	// Braced initialization
	return Vector2{static_cast<float>(point->x), static_cast<float>(point->y)};
}

////////////////////////////////////////////////// PhysicsEngine //////////////////////////////////////////////////////////////

void PhysicsEngine::UpdatePhysics(PhysicsObject* first, const float delta_time, const float simulation_speed) const {
	// Temporary variables
	PhysicsObject* current = first;
	PhysicsObject* currentMatcher = first;

	// Go through all objects
	while (current != nullptr) {
		currentMatcher = first;
		while (currentMatcher != nullptr) {
			if (currentMatcher == current) {
				currentMatcher = currentMatcher->next;
				continue; // Skip itself
			}

			Vector2* pos1 = current->GetLocation();
			Vector2* pos2 = currentMatcher->GetLocation();
			Vector2 dir = *pos2 - *pos1;

			/* Newtons law of gravity
				  m1 * m2
			F = G --------
				    r^2
			*/
			const float forceStrength = static_cast<float>(6.672f * pow(10, -11)) * (static_cast<float>(current->GetMass() * static_cast<float>(currentMatcher->GetMass())) / pow(DistanceDifference(current, currentMatcher), 2));
			dir = dir.SetMag(forceStrength / static_cast<float>(current->GetMass()));

			current->ApplyForce(dir);

			currentMatcher = currentMatcher->next;
		}
			
		// Set velocity
		// v = v0 + a * delta-t
		current->SetVelocity(*current->GetVelocity() + current->GetAcceleration()->SetMag(delta_time * simulation_speed));

		// Set location --
		// s = s0 + v * delta-t
		current->SetLocation(*current->GetLocation() + *current->GetVelocity());

		// Reset acceleration
		current->GetAcceleration()->SetMag(0);

		//current->Update();
		current = current->next;
	}
}

