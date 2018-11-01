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
	const double forceStrength = CalculateForceBetweenObjects(object->GetLocation(), &target_position, object->GetMass(), object->GetMass() * DistanceDifference(pos1, &target_position) * amplified_force);
	dir = dir.SetMag(static_cast<float>(forceStrength / object->GetMass()));

	object->ApplyForce(dir);

	std::cout << DistanceDifference(pos1, &target_position) << std::endl;
}

////////////////////////////////////////////////// PhysicsEngine //////////////////////////////////////////////////////////////

int PhysicsEngine::CollisionManagement(PhysicsObject* first, PhysicsObject* second, Universe* universe) const {
	std::cout << "X: " << first->GetX() << std::endl;
	std::cout << "SX: " << second->GetX() << std::endl;
	const double distance = DistanceDifference(first, second);
	std::cout << "------" << std::endl;

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

		// Calculate work
		// KE = 1/2 * m * v2
		// Calc first object's work
		const Vector2 firstWork(
			pow(first->GetVelocity()->x, 2) * static_cast<double>(1)/static_cast<double>(2) * first->GetMass(),
			pow(first->GetVelocity()->y, 2) * static_cast<double>(1)/static_cast<double>(2) * first->GetMass()
		); 
		// Calc second object's work
		const Vector2 secondWork(
			pow(second->GetVelocity()->x, 2) * static_cast<double>(1)/static_cast<double>(2) * second->GetMass(),
			pow(second->GetVelocity()->y, 2) * static_cast<double>(1)/static_cast<double>(2) * second->GetMass()
		);

		std::cout << "===STATS===\nV2 x: " << pow(second->GetVelocity()->x, 2) << "\nV2 y: " << pow(second->GetVelocity()->y, 2) << "\nOther calculation: " << 
			static_cast<double>(1)/static_cast<double>(2) * second->GetMass() << "\n===========" << std::endl;


		//const auto secondWork = *second->GetVelocity() * second->GetVelocity()->SetMag(static_cast<double>(1)/2 * second->GetMass());

		// Add together kinetic energy
		const Vector2 newWork = firstWork + secondWork;

		std::cout << "First Work: " << firstWork << std::endl;
		std::cout << "Second Work: " << secondWork << std::endl;
		std::cout << "Work: " << newWork << std::endl;

		// Calc velocity from work 
		// (2KE / m))**(1/2) = v
		Vector2 newVelocity;
		newVelocity.x = std::sqrt(2 * newWork.x / newMass);
		newVelocity.y = std::sqrt(2 * newWork.y / newMass);

		// Multiply velocity with -1 if heavier object has a velocity in negative direction
		if (first->GetMass() > second->GetMass()) {
			if (first->GetVelocity()->x < 0) {
				newVelocity.x = newVelocity.x * -1;
			}
			if (first->GetVelocity()->y < 0) {
				newVelocity.y = newVelocity.y * -1;
			}
		} else {
			if (second->GetVelocity()->x < 0) {
				newVelocity.x = newVelocity.x * -1;
			}
			if (second->GetVelocity()->y < 0) {
				newVelocity.y = newVelocity.y * -1;
			}
		}

		std::cout << "Velocity: " << newVelocity << std::endl;

		// Apply new velocity
		//object->SetVelocity(newVelocity);

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

		current = current->next;
	}
}

