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
	void DisplaySettings();
	void Update();

	Vector2* getLocation() { return &location; };
	Vector2* getVelocity() { return &velocity; };
	Vector2* getAcceleration() { return &acceleration; };
	SDL_Color* getColor() { return &color; };
	int getRadius() { return radius; };
	float getX() { return location.x(); };
	float getY() { return location.y(); };
	float getMass() { return mass; };

	void setColor(int r = -1, int g = -1, int b = -1, int a = -1);
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

	void AddToQueue(PhysicsObject* object);
	Vector2 SDL_Point_to_Vec2(SDL_Point* point);
	float DistanceDifference(PhysicsObject* point, PhysicsObject* pointTwo);
	float DistanceDifference(Vector2* point, Vector2* pointTwo);
	PhysicsObject* GetObjectOnPosition(Vector2* location);
	PhysicsObject* SummonObject(SDL_Point* position, int radius, int mass, SDL_Color* color);
	void UpdatePhysics();
	void UpdateGraphics();
};

