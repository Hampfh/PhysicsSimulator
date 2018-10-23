#include "Vector.h"

Vector2::Vector2() {
	x = NULL;
	y = NULL;
}

Vector2::Vector2(const float x, const float y) : x(x), y(y) {

}

Vector2 Vector2::operator+ (Vector2 secondVec) const {
	return Vector2{
		this->x + secondVec.x,
		this->y + secondVec.y
	};
}
Vector2 Vector2::operator- (Vector2 secondVec) const {
	return Vector2{
		this->x - secondVec.x,
		this->y - secondVec.y
	};
}
Vector2 Vector2::operator* (Vector2 secondVec) const {
	return Vector2{
		this->x * secondVec.x,
		this->y * secondVec.y
	};
}
Vector2 Vector2::operator/ (Vector2 secondVec) const {
	return Vector2{
		this->x / secondVec.x,
		this->y / secondVec.y
	};
}

bool Vector2::operator< (Vector2 secondVec) const {
	return (x + y < secondVec.x + secondVec.y);
}

bool Vector2::operator> (Vector2 secondVec) const {
	return (x + y > secondVec.x + secondVec.y);
}

bool Vector2::operator== (Vector2 secondVec) const {
	return (x == secondVec.x && y == secondVec.y);
}

void Vector2::Limit(const int limit) {
	if (this->x < 0.0f && abs(this->x) > limit) {
		this->x = limit * -1;
	}
	else if (this->x > limit) {
		this->x = limit;
	}

	if (this->y < 0.0f && abs(this->y) > limit) {
		this->y = limit * -1.0f;
	}
	else if (this->y > limit) {
		this->y = limit;
	}
}

Vector2 Vector2::SetMag(float magnitude) {
	return Vector2{
		this->x = this->x * magnitude,
		this->y = this->y * magnitude
	};
}

Vector2 Vector2::Divide(const float divider) {
	return Vector2{this->x = this->x / divider,
		this->y = this->y / divider};
}

std::ostream& operator<<(std::ostream& os, Vector2 vector) {
	os << "Vector2: x = " << vector.x <<
		" y = " << vector.y <<
		"\n";
	return os;
}