#include "Vector.h"

Vector2::Vector2() {
	_x = NULL;
	_y = NULL;
}

Vector2::Vector2(float x, float y) : _x(x), _y(y) {

}

void Vector2::Set(float x, float y) {
	_x = x;
	_y = y;
}

Vector2 Vector2::operator+ (Vector2 secondVec) {
	return Vector2(
		this->_x + secondVec._x,
		this->_y + secondVec._y
	);
}
Vector2 Vector2::operator- (Vector2 secondVec) {
	return Vector2(
		this->_x - secondVec._x,
		this->_y - secondVec._y
	);
}
Vector2 Vector2::operator* (Vector2 secondVec) {
	return Vector2(
		this->_x * secondVec._x,
		this->_y * secondVec._y
	);
}
Vector2 Vector2::operator/ (Vector2 secondVec) {
	return Vector2(
		this->_x / secondVec._x,
		this->_y / secondVec._y
	);
}

bool Vector2::operator< (Vector2 secondVec) {
	if (_x + _y < secondVec._x + secondVec._y)
		return true;
	return false;
}

bool Vector2::operator> (Vector2 secondVec) {
	if (_x + _y > secondVec._x + secondVec._y)
		return true;
	return false;
}

bool Vector2::operator== (Vector2 secondVec) {
	if (_x == secondVec._x &&
		_y == secondVec._y)
		return true;	
	return false;
}

void Vector2::limit(int limit) {
	if (this->_x < 0.0f && abs(this->_x) > limit) {
		this->_x = limit * -1;
	}
	else if (this->_x > limit) {
		this->_x = limit;
	}

	if (this->_y < 0.0f && abs(this->_y) > limit) {
		this->_y = limit * -1.0f;
	}
	else if (this->_y > limit) {
		this->_y = limit;
	}
}

Vector2 Vector2::SetMag(float magnitude) {
	return Vector2{
		this->_x = this->_x * magnitude,
		this->_y = this->_y * magnitude
	};
}

Vector2 Vector2::Divide(const float divider) {
	return Vector2{this->_x = this->_x / divider,
		this->_y = this->_y / divider};
}


Vector3::Vector3() {
	_x = NULL;
	_y = NULL;
	_z = NULL;
}

Vector3::Vector3(float x, float y, float z) : _x(x), _y(y), _z(z) {

}

void Vector3::Set(float x, float y, float z) {
	_x = x;
	_y = y;
	_z = z;
}

Vector3 Vector3::operator+ (Vector3 secondVec) {
	return Vector3(
		this->_x + secondVec._x,
		this->_y + secondVec._y,
		this->_z + secondVec._z
	);
}
Vector3 Vector3::operator- (Vector3 secondVec) {
	return Vector3(
		this->_x - secondVec._x,
		this->_y - secondVec._y,
		this->_z - secondVec._z
	);
}
Vector3 Vector3::operator* (Vector3 secondVec) {
	return Vector3(
		this->_x * secondVec._x,
		this->_y * secondVec._y,
		this->_z * secondVec._z
	);
}
Vector3 Vector3::operator/ (Vector3 secondVec) {
	return Vector3(
		this->_x / secondVec._x, 
		this->_y / secondVec._y, 
		this->_z / secondVec._z
	);
}

bool Vector3::operator< (Vector3 secondVec) {
	if (_x + _y + _z < secondVec._x + secondVec._y + secondVec._z)
		return true;
	return false;
}

bool Vector3::operator> (Vector3 secondVec) {
	if (_x + _y + _z > secondVec._x + secondVec._y + secondVec._z)
		return true;
	return false;
}

bool Vector3::operator== (Vector3 secondVec) {
	if (_x == secondVec._x &&
		_y == secondVec._y &&
		_z == secondVec._z)
		return true;
	return false;
}

Vector4::Vector4() {
	_x = NULL;
	_y = NULL;
	_z = NULL;
}

Vector4::Vector4(float x, float y, float z, float w) : _x(x), _y(y), _z(z), _w(w) {

}

void Vector4::Set(float x, float y, float z, float w) {
	_x = x;
	_y = y;
	_z = z;
	_w = w;
}

Vector4 Vector4::operator+ (Vector4 secondVec) {
	return Vector4(
		this->_x + secondVec._x,
		this->_y + secondVec._y,
		this->_z + secondVec._z,
		this->_w + secondVec._w
	);
}
Vector4 Vector4::operator- (Vector4 secondVec) {
	return Vector4(
		this->_x - secondVec._x,
		this->_y - secondVec._y,
		this->_z - secondVec._z,
		this->_w - secondVec._w
	);
}
Vector4 Vector4::operator* (Vector4 secondVec) {
	return Vector4(
		this->_x * secondVec._x,
		this->_y * secondVec._y,
		this->_z * secondVec._z,
		this->_w * secondVec._w
	);
}
Vector4 Vector4::operator/ (Vector4 secondVec) {
	return Vector4(
		this->_x / secondVec._x,
		this->_y / secondVec._y,
		this->_z / secondVec._z,
		this->_w / secondVec._w
	);
}

bool Vector4::operator< (Vector4 secondVec) {
	if (_x + _y + _z + _w < secondVec._x + secondVec._y + secondVec._z + secondVec._w)
		return true;
	return false;
}

bool Vector4::operator> (Vector4 secondVec) {
	if (_x + _y + _z + _w > secondVec._x + secondVec._y + secondVec._z + secondVec._w)
		return true;
	return false;
}

bool Vector4::operator== (Vector4 secondVec) {
	if (_x == secondVec._x &&
		_y == secondVec._y &&
		_z == secondVec._z &&
		_w == secondVec._w)
		return true;
	return false;
}

std::ostream& operator<<(std::ostream& os, Vector2 vector) {
	os << "Vector2: x = " << vector._x <<
		" y = " << vector._y <<
		"\n";
	return os;
}

std::ostream& operator<<(std::ostream& os, Vector3 vector) {
	os << "Vector3: x = " << vector._x <<
		" y = " << vector._y <<
		" z = " << vector._z <<
		"\n";
	return os;
}

std::ostream& operator<<(std::ostream& os, Vector4 vector) {
	os << "Vector4: x = " << vector._x <<
		" y = " << vector._y <<
		" z = " << vector._z <<
		" w = " << vector._w <<
		"\n";
	return os;
}