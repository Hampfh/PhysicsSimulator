#pragma once
#include <ostream>
#include <functional>

// Vector functions create by Hampus Hallkvist
// Apache License 2.0

class Vector2 {
	friend std::ostream& operator<<(std::ostream& os, Vector2 vector);
public:
	float x;
	float y;

	Vector2();
	Vector2(float x, float y);

	Vector2 operator+ (Vector2 secondVec) const;
	Vector2 operator- (Vector2 secondVec) const;
	Vector2 operator* (Vector2 secondVec) const;
	Vector2 operator/ (Vector2 secondVec) const;
	bool operator< (Vector2 secondVec) const;
	bool operator> (Vector2 secondVec) const;
	bool operator== (Vector2 secondVec) const;
	double GetMagnitude() const;
	void Limit(int limit);
	Vector2 Multiply(float factor);
	Vector2 Divide(float numerator);

};

std::ostream& operator<<(std::ostream& os, Vector2 vector);