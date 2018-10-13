#include "PhysicsEngine.h"

SDL_Renderer* Core::renderer;

////////////////////////////////////////////////// PhysicsObject //////////////////////////////////////////////////////////////

PhysicsObject::PhysicsObject(SDL_Point* position, int radius, int mass, SDL_Color* color) 
: radius(radius), mass(mass), color(*color) {
	location.Set(position->x, position->y);
	velocity.Set(0,0);
	acceleration.Set(0, 0);
}

void PhysicsObject::ApplyForce(Vector2 force) {
	//std::cout << "Current: " << acceleration << " + " << force;
	acceleration = acceleration + force;
	//std::cout << " " << acceleration << std::endl;
}

void PhysicsObject::DrawCircle() {
	for (double dy = 1; dy <= radius; dy += 1.0) {
		double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
		SDL_SetRenderDrawColor(Core::renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawLine(Core::renderer, location.x() - dx, location.y() + dy - radius, location.x() + dx, location.y() + dy - radius);
		SDL_RenderDrawLine(Core::renderer, location.x() - dx, location.y() - dy + radius, location.x() + dx, location.y() - dy + radius);
	}
}

// Updates the single object
void PhysicsObject::Update() {
	velocity = velocity + acceleration;
	location = location + velocity;
	DrawCircle();
	acceleration.setMag(0);
}

////////////////////////////////////////////////// PhysicsEngine //////////////////////////////////////////////////////////////

PhysicsEngine::PhysicsEngine(int simulationWidth, int simulationHeight) : simulationWidth(simulationWidth), simulationHeight(simulationHeight) {

}


PhysicsEngine::~PhysicsEngine() {
	// Delete queue
	PhysicsObject* current = firstObject;
	while (current != nullptr) {
		current = current->next;
		delete firstObject;
		firstObject = current;
	}
}

void PhysicsEngine::UpdatePhysics(SDL_Event* event) {
	// Temporary variables
	PhysicsObject* current = firstObject;
	PhysicsObject* currentMatcher = firstObject;

	// Go through all objects
	while (current != nullptr) {
		currentMatcher = firstObject;
		while (currentMatcher != nullptr) {
			if (currentMatcher == current) {
				currentMatcher = currentMatcher->next;
				continue; // Continues if matches with itself
			}

			Vector2* pos1 = current->getLocation();
			Vector2* pos2 = currentMatcher->getLocation();
			Vector2 dir = *pos2 - *pos1;

			// Newtons law of gravity
			float forceStrength = 6.672 * pow(10, -11) * ((current->getMass() * currentMatcher->getMass()) / pow(DistanceDifference(current, currentMatcher), 2));

			dir.setMag(forceStrength * 100000);

			current->ApplyForce(dir);

			std::cout << dir << std::endl;

			currentMatcher = currentMatcher->next;
		}
		current->Update();
		current = current->next;
	}

	SDL_Delay(1);
}

void PhysicsEngine::AddToQueue(PhysicsObject* object) {
	if (firstObject == nullptr) {
		firstObject = object;
		lastObject = object;
	}
	else {
		lastObject->next = object;
		lastObject = object;
	}
}

float PhysicsEngine::DistanceDifference(PhysicsObject* point, PhysicsObject* pointTwo) {
	float differenceX = abs(point->getX() - pointTwo->getX());
	float differenceY = abs(point->getY() - pointTwo->getY());

	return sqrt(pow(differenceX, 2) + pow(differenceY, 2));
}

PhysicsObject* PhysicsEngine::SummonObject(SDL_Point* position, int radius, int mass, SDL_Color* color) {
	PhysicsObject* newObject = new PhysicsObject(position, radius, mass, color);
	AddToQueue(newObject);
	return newObject;
}