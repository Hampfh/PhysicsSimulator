#pragma once
#include "Core.h"
#include <cmath>

class PhysicsObject {
private:
	SDL_Point position;
	int radius;
	int mass;
	SDL_Color color;
public:
	PhysicsObject* next = nullptr;
public:
	PhysicsObject(SDL_Point* position, int radius, int mass, SDL_Color* color);
	void DrawCircle(int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
};

class PhysicsEngine {
private:
	int G; // GravitationalConstant

	// Linked list
	PhysicsObject* firstObject = nullptr;
	PhysicsObject* lastObject = nullptr;

public:
	PhysicsEngine(int GravitationalConstant = 6.67408*pow(10, -11));
	~PhysicsEngine();

	void RunPhysics();
	void AddToQueue(PhysicsObject* object);
	PhysicsObject* SummonObject(SDL_Point* position, int radius, int mass, SDL_Color* color);
};

