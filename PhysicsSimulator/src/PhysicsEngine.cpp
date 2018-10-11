#include "PhysicsEngine.h"

SDL_Renderer* Core::renderer;

void Vector::Sub(Vector vector) {
	this->x = this->x - vector.x;
	this->y = this->y - vector.y;
}

void Vector::Add(Vector vector) {
	this->x = this->x + vector.x;
	this->y = this->y + vector.y;
}

PhysicsObject::PhysicsObject(SDL_Point* position, int radius, int mass, SDL_Color* color) 
: radius(radius), mass(mass), color(*color) {
	this->position.x = position->x;
	this->position.y = position->y;
}

void PhysicsObject::DrawCircle(int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	for (double dy = 1; dy <= radius; dy += 1.0) {
		double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
		SDL_SetRenderDrawColor(Core::renderer, r, g, b, a);
		SDL_RenderDrawLine(Core::renderer, cx - dx, cy + dy - radius, cx + dx, cy + dy - radius);
		SDL_RenderDrawLine(Core::renderer, cx - dx, cy - dy + radius, cx + dx, cy - dy + radius);
	}
}

PhysicsEngine::PhysicsEngine() {

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
	int x;
	int y;
	SDL_GetGlobalMouseState(&x, &y);
	Vector mouse;
	mouse.x = x;
	mouse.y = y;
	mouse.Sub(firstObject->position);
	Vector acceleration = mouse;

	Vector velocity;
	velocity.Add(acceleration);
	firstObject->position.Add(velocity);
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
	std::cout << "ENDL" << std::endl;
}

PhysicsObject* PhysicsEngine::SummonObject(SDL_Point* position, int radius, int mass, SDL_Color* color) {
	PhysicsObject* newObject = new PhysicsObject(position, radius, mass, color);
	AddToQueue(newObject);
	return newObject;
}