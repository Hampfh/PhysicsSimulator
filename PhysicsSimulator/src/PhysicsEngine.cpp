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

void PhysicsObject::DisplaySettings() {

}

// Updates the single object
void PhysicsObject::Update() {
	velocity = velocity + acceleration;
	location = location + velocity;
	// Reset color 
	color.r = 20;
	color.g = 20;
	color.b = 20;
	acceleration.setMag(0);
}

void PhysicsObject::setColor(int r, int g, int b, int a) {
	if (r >= 0) this->color.r = r;
	if (g >= 0) this->color.g = g;
	if (b >= 0) this->color.b = b;
	if (a >= 0) this->color.a = a;
}

////////////////////////////////////////////////// PhysicsEngine //////////////////////////////////////////////////////////////

PhysicsEngine::PhysicsEngine(int simulationWidth, int simulationHeight) 
	: simulationWidth(simulationWidth), simulationHeight(simulationHeight) {

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

void PhysicsEngine::UpdatePhysics() {
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

			/* Newtons law of gravity
				  m1 * m2
			F = G --------
				    r^2
			*/
			float forceStrength = 6.672 * pow(10, -11) * ((current->getMass() * currentMatcher->getMass()) / pow(DistanceDifference(current, currentMatcher), 2));

			dir.setMag(forceStrength * 100000);

			current->ApplyForce(dir);

			currentMatcher = currentMatcher->next;
		}
		current->Update();
		current = current->next;
	}

	SDL_Delay(1);
}

void PhysicsEngine::UpdateGraphics() {
	PhysicsObject* current = firstObject;
	while (current != nullptr) {
		current->DrawCircle();
		current = current->next;
	}
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

Vector2 PhysicsEngine::SDL_Point_to_Vec2(SDL_Point* point) {
	return Vector2(point->x, point->y);
}

float PhysicsEngine::DistanceDifference(PhysicsObject* point, PhysicsObject* pointTwo) {
	float differenceX = abs(point->getX() - pointTwo->getX());
	float differenceY = abs(point->getY() - pointTwo->getY());

	return sqrt(pow(differenceX, 2) + pow(differenceY, 2));
}
float PhysicsEngine::DistanceDifference(Vector2* point, Vector2* pointTwo) {
	float differenceX = abs(point->x() - pointTwo->x());
	float differenceY = abs(point->y() - pointTwo->y());

	return sqrt(pow(differenceX, 2) + pow(differenceY, 2));
}

PhysicsObject* PhysicsEngine::SummonObject(SDL_Point* position, int radius, int mass, SDL_Color* color) {
	PhysicsObject* newObject = new PhysicsObject(position, radius, mass, color);
	AddToQueue(newObject);
	return newObject;
}

PhysicsObject* PhysicsEngine::GetObjectOnPosition(Vector2* location) {
	// Temporary variables
	PhysicsObject* current = firstObject;

	// Go through all objects
	while (current != nullptr) {
		
		int distanceBetween = DistanceDifference(current->getLocation(), location);
		if (distanceBetween <= current->getRadius()) {
			return current;
		}

		current = current->next;
	}
	return nullptr;
}