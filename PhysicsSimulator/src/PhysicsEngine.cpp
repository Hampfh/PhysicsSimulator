#include "PhysicsEngine.h"

float DistanceDifference(PhysicsObject* point, PhysicsObject* point_two) {
	const float differenceX = abs(point->GetX() - point_two->GetX());
	const float differenceY = abs(point->GetY() - point_two->GetY());

	return sqrt(pow(differenceX, 2) + pow(differenceY, 2));
}
float DistanceDifference(Vector2* point, Vector2* point_two) {
	const float differenceX = abs(point->x() - point_two->x());
	const float differenceY = abs(point->y() - point_two->y());

	return sqrt(pow(differenceX, 2) + pow(differenceY, 2));
}

Vector2 SDLPointToVec2(SDL_Point* point) {
	// Braced initialization
	return Vector2{static_cast<float>(point->x), static_cast<float>(point->y)};
}


////////////////////////////////////////////////// PhysicsEngine //////////////////////////////////////////////////////////////

void PhysicsEngine::UpdatePhysics(PhysicsObject* first, float timeInterval) const {
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
			const float forceStrength = static_cast<float>(6.672f * pow(10, -11)) * ((current->GetMass() * currentMatcher->GetMass()) / pow(DistanceDifference(current, currentMatcher), 2));
			dir = dir.SetMag(forceStrength * 100000 / current->GetMass());

			current->ApplyForce(dir);

			/*
			if (showForces_) {
				SDL_SetRenderDrawColor(Core::renderer_, 20, 20, 20, 255);
				SDL_RenderDrawLine(Core::renderer_, current->GetLocation()->x(), current->GetLocation()->y(), 
					pos2->x(), pos2->y());	
			}*/

			currentMatcher = currentMatcher->next;
		}
		// calc F
		const auto firstForce = current->GetFirstForce();
		auto prevForce = firstForce;
		auto currentForce = firstForce;
		int listCount = 0;
		Vector2 totalForce;

		while (currentForce != nullptr) {
			totalForce = totalForce + currentForce->force;
			listCount++;
			currentForce = currentForce->next;
			delete prevForce;
			prevForce = currentForce;
		}
		totalForce = totalForce.Divide(listCount);

		std::cout << totalForce.x() << " : " << totalForce.y() << std::endl;

		// Update acceleration
		// a = F / m
		Vector2 acceleration = totalForce.Divide(current->GetMass());

		// Update velocity
		// v = v0 + at
		Vector2 velocity = *current->GetVelocity() + acceleration.SetMag(timeInterval);

		// Update location
		// s = v * t
		Vector2 location = velocity.SetMag(timeInterval);

		current->SetAcceleration(acceleration);
		current->SetVelocity(velocity);
		current->SetLocation(location);

		//current->SetAcceleration(current->GetForce()->Divide(current->GetMass()));

		//current->SetVelocity(*current->GetVelocity() + current->GetAcceleration()->SetMag(timeInterval));
		// Old code
		//current->SetVelocity(*current->GetVelocity() + *current->GetAcceleration());

		//current->SetLocation(*current->GetLocation() + *current->GetVelocity());
		// Set location --

		current->Update();
		current = current->next;
	}
}

