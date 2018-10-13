#pragma once
#include <ostream>
#include <functional>

class Vector2 {
private:
	friend std::ostream& operator<<(std::ostream& os, Vector2 vector);
	float _x = 0.0f;
	float _y = 0.0f;
public:
	Vector2();
	Vector2(float x, float y);
	
	void Set(float newX, float newY);

	float x() const { return _x; } // Returns the value of x
	float y() const { return _y; } // Returns the value of y

	Vector2 operator+ (Vector2 secondVec);
	Vector2 operator- (Vector2 secondVec);
	Vector2 operator* (Vector2 secondVec);
	Vector2 operator/ (Vector2 secondVec);
	bool operator< (const Vector2 secondVec);
	bool operator> (const Vector2 secondVec);
	bool operator== (Vector2 secondVec);
	void limit(int limit);
	void setMag(float magnitude);
};

class Vector3 {
private:
	friend std::ostream& operator<<(std::ostream& os, Vector3 vector);
	float _x = 0.0f;
	float _y = 0.0f;
	float _z = 0.0f;
public:
	Vector3();
	Vector3(float x, float y, float z);

	void Set(float newX, float newY, float newZ);

	float x() const { return _x; } // Returns the value of x
	float y() const { return _y; } // Returns the value of y
	float z() const { return _z; } // Returns the value of z

	Vector3 operator+ (Vector3 secondVec);
	Vector3 operator- (Vector3 secondVec);
	Vector3 operator* (Vector3 secondVec);
	Vector3 operator/ (Vector3 secondVec);
	bool operator< (Vector3 secondVec);
	bool operator> (Vector3 secondVec);
	bool operator== (Vector3 secondVec);
};

class Vector4 {
private:
	friend std::ostream& operator<<(std::ostream& os, Vector4 vector);
	float _x = 0.0f;
	float _y = 0.0f;
	float _z = 0.0f;
	float _w = 0.0f;
public:
	Vector4();
	Vector4(float x, float y, float z, float w);

	void Set(float x, float y, float z, float w);

	float x() const { return _x; } // Returns the value of x
	float y() const { return _y; } // Returns the value of y
	float z() const { return _z; } // Returns the value of z
	float w() const { return _w; } // Returns the value of w

	Vector4 operator+ (Vector4 secondVec);
	Vector4 operator- (Vector4 secondVec);
	Vector4 operator* (Vector4 secondVec);
	Vector4 operator/ (Vector4 secondVec);
	bool operator< (Vector4 secondVec);
	bool operator> (Vector4 secondVec);
	bool operator== (Vector4 secondVec);
};

std::ostream& operator<<(std::ostream& os, Vector2 vector);
std::ostream& operator<<(std::ostream& os, Vector3 vector);
std::ostream& operator<<(std::ostream& os, Vector4 vector);


