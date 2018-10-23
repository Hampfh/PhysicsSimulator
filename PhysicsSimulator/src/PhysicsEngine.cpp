#include "PhysicsEngine.h"

float DistanceDifference(PhysicsObject* point, PhysicsObject* point_two) {
	const float differenceX = abs(point->GetX() - point_two->GetX());
	const float differenceY = abs(point->GetY() - point_two->GetY());

	return sqrt(pow(differenceX, 2) + pow(differenceY, 2));
}
float DistanceDifference(Vector2* point, Vector2* point_two) {
	const float differenceX = abs(point->x - point_two->x);
	const float differenceY = abs(point->y - point_two->y);

	return sqrt(pow(differenceX, 2) + pow(differenceY, 2));
}

Vector2 SDLPointToVec2(SDL_Point* point) {
	// Braced initialization
	return Vector2{static_cast<float>(point->x), static_cast<float>(point->y)};
}

////////////////////////////////////////////////// PhysicsEngine //////////////////////////////////////////////////////////////

void PhysicsEngine::UpdatePhysics(PhysicsObject* first, const float time_interval) const {
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
		current->SetVelocity(*current->GetVelocity() + current->GetAcceleration()->SetMag(time_interval));

		// Set location --
		// s = s0 + v + delta-t
		current->SetLocation(*current->GetLocation() + current->GetVelocity()->SetMag(time_interval));


		// Reset acceleration
		current->GetAcceleration()->SetMag(0);
		

		//current->Update();
		current = current->next;
	}
}

