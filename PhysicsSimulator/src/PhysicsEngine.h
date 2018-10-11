#pragma once
#include "Core.h"
#include <cmath>

struct Vector {
	int x = 0;
	int y = 0;

	void Sub(Vector vector);
	void Add(Vector vector);
};

class PhysicsObject {
public:
	Vector position;
	int radius;
	int mass;
	SDL_Color color;
	Vector acceleration;
	Vector velocity;
public:
	PhysicsObject* next = nullptr;
public:
	PhysicsObject(SDL_Point* position, int radius, int mass, SDL_Color* color);
	void DrawCircle(int cx, int cy, int radius, Uint8 r, Uint8 g, Uint8 b, Uint8 a);
};

class PhysicsEngine {
private:
	// Linked list
	PhysicsObject* firstObject = nullptr;
	PhysicsObject* lastObject = nullptr;
public:
	PhysicsEngine(); 
	~PhysicsEngine();

	void UpdatePhysics();
	void AddToQueue(PhysicsObject* object);
	PhysicsObject* SummonObject(SDL_Point* position, int radius, int mass, SDL_Color* color);
};

