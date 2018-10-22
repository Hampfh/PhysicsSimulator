#pragma once
#include "PhysicsEngine.h"
#include "includes/Vector.h"
#include <string>

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

	double radius_;
	double mass_;

	Vector2 location_;
	Vector2 velocity_;
	Vector2 acceleration_;
	
	SDL_Color color_;
	SDL_Color defaultColor_;

	SDL_Rect settingsBox_;

public:
	PhysicsObject* next = nullptr;

	PhysicsObject(int id, SDL_Point* position, double radius, double mass, SDL_Color* color);
	void ApplyForce(Vector2 force);
	TextPackage PrepareObjectSettings();

	Vector2* GetLocation() { return &location_; };
	Vector2* GetVelocity() { return &velocity_; };
	Vector2* GetAcceleration() { return &acceleration_; };
	SDL_Color* GetColor() { return &color_; };
	double GetRadius() const { return radius_; };
	double GetMass() const { return mass_; };
	float GetX() const { return location_.x(); };
	float GetY() const { return location_.y(); };
	int GetId() const { return objectId_; };

	void SetForceCount(int force_count);
	void SetLocation(Vector2 location);
	void SetVelocity(Vector2 velocity);
	void SetAcceleration(Vector2 acceleration);
	void SetColor(int r = -1, int g = -1, int b = -1, int a = -1);
	void SetRadius(int radius);
	void SetMass(float mass);
	void ResetColor();
};

class Universe {
	PhysicsObject* firstObject_ = nullptr;
	PhysicsObject* lastObject_ = nullptr;

	float* metersPerPixel_ = nullptr;

	int* originX_;
	int* originY_;

	void InsertObject(PhysicsObject* object);
public:
	Universe(float* meters_per_pixel, int* origin_x, int* origin_y);
	~Universe();

	PhysicsObject* GetFirst() const;
	PhysicsObject* GetLast() const;
	PhysicsObject* GetObjectOnPosition(Vector2* location) const;
	PhysicsObject* GetObjectWithId(int id) const;
	PhysicsObject* SummonObject(SDL_Point* position, double radius, double mass, SDL_Color* color);
};