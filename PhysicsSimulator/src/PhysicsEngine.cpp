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

void Vector::Limit(int limit) {
	if (this->x < 0 && abs(this->x) > limit) {
		this->x = limit * -1;
	} else if (this->x > limit) {
		this->x = limit;
	}

	if (this->y < 0 && abs(this->y) > limit) {
		this->y = limit * -1;
	}
	else if (this->y > limit) {
		this->y = limit;
	}
}

void Vector::SetMag(float magnitude) {
	this->x = this->x * magnitude / 1;
	this->y = this->y * magnitude / 1;
}

PhysicsObject::PhysicsObject(SDL_Point* position, int radius, int mass, SDL_Color* color) 
: radius(radius), mass(mass), color(*color) {
	this->position.x = position->x;
	this->position.y = position->y;
}

void PhysicsObject::DrawCircle() {
	for (double dy = 1; dy <= radius; dy += 1.0) {
		double dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
		SDL_SetRenderDrawColor(Core::renderer, color.r, color.g, color.b, color.a);
		SDL_RenderDrawLine(Core::renderer, position.x - dx, position.y + dy - radius, position.x + dx, position.y + dy - radius);
		SDL_RenderDrawLine(Core::renderer, position.x - dx, position.y - dy + radius, position.x + dx, position.y - dy + radius);
	}
}

PhysicsEngine::PhysicsEngine() {
	mouse = new Vector;
	acceleration = new Vector;
	velocity = new Vector;
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
	SDL_GetMouseState(&x, &y);
	
	mouse->x = x;
	mouse->y = y;
	mouse->Sub(firstObject->position);
	mouse->SetMag(0.01);
	std::cout << "MOUSE: " << mouse->x << " : " << mouse->y << std::endl;
	std::cout << "FirstObject: " << firstObject->position.x << " : " << firstObject->position.y << std::endl;
	acceleration = mouse;

	velocity->Add(*acceleration);
	velocity->Limit(5);
	firstObject->position.Add(*velocity);
	firstObject->DrawCircle();
	std::cout << "VELOCITY: " << velocity->x << " : " << velocity->y << std::endl;
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
	std::cout << "ENDL" << std::endl;
}

PhysicsObject* PhysicsEngine::SummonObject(SDL_Point* position, int radius, int mass, SDL_Color* color) {
	PhysicsObject* newObject = new PhysicsObject(position, radius, mass, color);
	AddToQueue(newObject);
	return newObject;
}