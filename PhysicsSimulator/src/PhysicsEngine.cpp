#include "PhysicsEngine.h"

double CalculateForceBetweenObjects(Vector2* pos1, Vector2* pos2, double m1, double m2) {	
	/* Newtons law of gravity
		  m1 * m2
	F = G --------
		    r^2
	*/
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
	const double forceStrength = CalculateForceBetweenObjects(object->GetLocation(), &target_position, object->GetMass(), object->GetMass() * DistanceDifference(pos1, &target_position) * amplified_force);
	dir = dir.Multiply(static_cast<float>(forceStrength / object->GetMass()));

	object->ApplyForce(dir);
}

////////////////////////////////////////////////// PhysicsEngine //////////////////////////////////////////////////////////////

int PhysicsEngine::CollisionManagement(PhysicsObject* first, PhysicsObject* second, Universe* universe) const {
	const double distance = DistanceDifference(first, second);

	// Collision has happened
	if (distance < first->GetRadius() + second->GetRadius()) {
		// Calculate center of mass
		const double massCenterX = (first->GetMass() * first->GetLocation()->x + second->GetMass() * second->GetLocation()->x) / (first->GetMass() + second->GetMass());
		const double massCenterY = (first->GetMass() * first->GetLocation()->y + second->GetMass() * second->GetLocation()->y) / (first->GetMass() + second->GetMass());

		// Calculate volumes
		const double firstObjectVolume = static_cast<double>(4)/3 * PI * pow(first->GetRadius(), 3);
		const double secondObjectVolume = static_cast<double>(4)/3 * PI * pow(second->GetRadius(), 3);
		// Merge volumes
		const double newVolume = firstObjectVolume + secondObjectVolume;
		// Merge masses
		const double newMass = first->GetMass() + second->GetMass();
		// Calculate new radius for new object
		const double newRadius = std::cbrt(newVolume / (static_cast<double>(4)/3 * PI));

		// New object position
		Vector2 newPos(massCenterX, massCenterY);

		SDL_Color color;
		color.r = 20;
		color.g = 20;
		color.b = 20;
		color.a = 255;

		// Create new object
		PhysicsObject* object = universe->SummonObject(&newPos, newRadius, newMass, &color);

		// Calculate kinetic energy
		// KE = 1/2 * m * v2
		// Calc first object's KE
		const double firstKineticEnergy = pow(first->GetVelocity()->GetMagnitude(), 2) * static_cast<double>(1)/static_cast<double>(2) * first->GetMass();
		Vector2 firstUnitVector = first->GetVelocity()->Divide(first->GetVelocity()->GetMagnitude());
		const Vector2 firstKeVector = firstUnitVector.Multiply(firstKineticEnergy);

		const double secondKineticEnergy = pow(second->GetVelocity()->GetMagnitude(), 2) * static_cast<double>(1)/static_cast<double>(2) * second->GetMass();
		Vector2 secondUnitVector = second->GetVelocity()->Divide(second->GetVelocity()->GetMagnitude());
		const Vector2 secondKeVector = secondUnitVector.Multiply(secondKineticEnergy);

		Vector2 totalKeVector = firstKeVector + secondKeVector;
		const double totalKeMag = totalKeVector.GetMagnitude();
		Vector2 totalUnitVector = totalKeVector.Divide(totalKeMag);

		const double newVelocity = std::sqrt(2 * totalKeMag / newMass);

		const Vector2 newVelocityVector = totalUnitVector.Multiply(newVelocity);
		
		// Apply new velocity
		std::cout << newVelocityVector << std::endl;
		object->SetVelocity(newVelocityVector);

		// Delete old objects
		universe->Delete(first);
		universe->Delete(second);

		return 1; // Collision has happened
	}
	return 0; // No changes
}

void PhysicsEngine::UpdatePhysics(Universe* universe, const float delta_time, const float simulation_speed) const {
	// Temporary variables
	PhysicsObject* current = universe->GetFirst();
	PhysicsObject* currentMatcher = universe->GetFirst();

	// Collision checking
	while (current != nullptr) {
		currentMatcher = universe->GetFirst();
		while (currentMatcher != nullptr) {
			if (currentMatcher == current) {
				currentMatcher = currentMatcher->next;
				continue;
			}

			if (CollisionManagement(current, currentMatcher, universe) == 1) {
				current = universe->GetFirst();
				currentMatcher = universe->GetFirst();
				continue;
			}
			currentMatcher = currentMatcher->next;
		}
		current = current->next;
	}

	// Reset current
	current = universe->GetFirst();

	// Go through all objects
	while (current != nullptr) {
		currentMatcher = universe->GetFirst();
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
			dir = dir.Multiply(forceStrength / static_cast<float>(current->GetMass()));

			current->ApplyForce(dir);

			currentMatcher = currentMatcher->next;
		}
			
		// Set velocity
		// v = v0 + a * delta-t
		current->SetVelocity(*current->GetVelocity() + current->GetAcceleration()->Multiply(delta_time * simulation_speed));

		// Set location --
		// s = s0 + v * delta-t
		current->SetLocation(*current->GetLocation() + *current->GetVelocity());

		// Reset acceleration
		current->GetAcceleration()->Multiply(0);

		current = current->next;
	}
}

