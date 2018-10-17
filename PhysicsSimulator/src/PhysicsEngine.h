#pragma once
#include "Core.h"
#include <cmath>
#include "includes/Vector.h"

const int setting_count = 3;

struct TextString {
	std::string text;
	std::string fontPath;
	int fontSize = 0;
	SDL_Rect settingTextBox;
};

struct TextPackage {
	int package_size = setting_count;
	TextString settings[setting_count];
	SDL_Rect* settingsBox = nullptr;
};

class PhysicsObject {
	const int objectId_;

	Vector2 location_;
	Vector2 velocity_;
	Vector2 acceleration_;
	int radius_;
	float mass_;
	SDL_Color color_;

	SDL_Rect settingsBox_;

public:
	PhysicsObject* next = nullptr;
	PhysicsObject(int id, SDL_Point* position, int radius, float mass, SDL_Color* color);
	void ApplyForce(Vector2 force);
	void DrawCircle() const;
	TextPackage PrepareObjectSettings();
	void Update();

	Vector2* GetLocation() { return &location_; };
	Vector2* GetVelocity() { return &velocity_; };
	Vector2* GetAcceleration() { return &acceleration_; };
	SDL_Color* GetColor() { return &color_; };
	int GetRadius() const { return radius_; };
	float GetMass() const { return mass_; };
	float GetX() const { return location_.x(); };
	float GetY() const { return location_.y(); };
	int GetId() const { return objectId_; };

	void SetColor(int r = -1, int g = -1, int b = -1, int a = -1);
};

class PhysicsEngine {
	// Linked list
	PhysicsObject* firstObject_ = nullptr;
	PhysicsObject* lastObject_ = nullptr;
	
	int simulationWidth_;
	int simulationHeight_;
	Vector2* mouse_ = nullptr;
public:
	PhysicsEngine(int simulation_width, int simulation_height);
	~PhysicsEngine();

	void AddToQueue(PhysicsObject* object);
	Vector2 SDLPointToVec2(SDL_Point* point);
	float DistanceDifference(PhysicsObject* point, PhysicsObject* point_two);
	float DistanceDifference(Vector2* point, Vector2* point_two);
	PhysicsObject* GetObjectOnPosition(Vector2* location);
	PhysicsObject* SummonObject(SDL_Point* position, int radius, int mass, SDL_Color* color);
	void UpdatePhysics();
	void UpdateGraphics();
};