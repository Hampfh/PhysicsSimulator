#include "PhysicsEngine.h"

SDL_Renderer* Core::renderer;

PhysicsObject::PhysicsObject(SDL_Point* position, int radius, int mass, SDL_Color* color) 
: position(*position), radius(radius), mass(mass), color(*color) {
}

void PhysicsObject::DrawCircle(int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a) {
	for (double dy = 1; dy <= radius; dy += 1.0) {
		double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
		SDL_SetRenderDrawColor(Core::renderer, r, g, b, a);
		SDL_RenderDrawLine(Core::renderer, cx - dx, cy + dy - radius, cx + dx, cy + dy - radius);
		SDL_RenderDrawLine(Core::renderer, cx - dx, cy - dy + radius, cx + dx, cy - dy + radius);
	}
}

PhysicsEngine::PhysicsEngine(int GravitationalConstant) : G(GravitationalConstant) {

}


PhysicsEngine::~PhysicsEngine() {

}

void PhysicsEngine::RunPhysics() {

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

PhysicsObject* PhysicsEngine::SummonObject(SDL_Point* position, int radius, int mass, SDL_Color* color) {
	PhysicsObject* newObject = new PhysicsObject(position, radius, mass, color);
	AddToQueue(newObject);
	return newObject;
}