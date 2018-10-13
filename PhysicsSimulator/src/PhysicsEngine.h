#pragma once
#include "Core.h"
#include <cmath>
#include "includes\Vector.h"

class PhysicsObject {
private:
	Vector2 location;
	Vector2 velocity;
	Vector2 acceleration;
	int radius;
	float mass;
	SDL_Color color;
public:
	PhysicsObject* next = nullptr;
public:
	PhysicsObject(SDL_Point* position, int radius, int mass, SDL_Color* color);
	void ApplyForce(Vector2 force);
	void DrawCircle();
	void Update();

	Vector2* getLocation() { return &location; };
	Vector2* getVelocity() { return &velocity; };
	Vector2* getAcceleration() { return &acceleration; };
	float getX() { return location.x(); };
	float getY() { return location.y(); };
	float getMass() { return mass; };
};

class PhysicsEngine {
private:
	// Linked list
	PhysicsObject* firstObject = nullptr;
	PhysicsObject* lastObject = nullptr;
	
	int simulationWidth;
	int simulationHeight;
	Vector2* mouse = nullptr;
public:
	PhysicsEngine(int simulationWidth, int simulationHeight);
	~PhysicsEngine();

	void UpdatePhysics(SDL_Event* event);
	void AddToQueue(PhysicsObject* object);
	float DistanceDifference(PhysicsObject* point, PhysicsObject* pointTwo);
	/**
	@Desciption: Inserts a sphere into the space
	@Return type: PhysicsObject*
	*/
	PhysicsObject* SummonObject(SDL_Point* position, int radius, int mass, SDL_Color* color);
};

