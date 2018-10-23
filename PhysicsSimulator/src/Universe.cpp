#include "Universe.h"
#include "Core.h"

PhysicsObject::PhysicsObject(const int id, SDL_Point* position, const double radius, const double mass, SDL_Color* color)
: objectId_(id), radius_(radius), mass_(mass), color_(*color) {
	location_.x = static_cast<float>(position->x);
	location_.y = static_cast<float>(position->y);
	velocity_.x = 0;
	velocity_.y = 0;
	acceleration_.x = 0;
	acceleration_.y = 0;
	defaultColor_ = color_;
}

void PhysicsObject::ApplyForce(const Vector2 force) {
	acceleration_ = acceleration_ + force;
}

TextPackage PhysicsObject::PrepareObjectSettings() {

	TextPackage package;
	package.settingsBox = &this->settingsBox_;

	int settingNum = 1;
	const int settingOffset = 5;
	const int settingFontSize = 15;

	TextString setting;

	// Create ID
	setting.settingTextBox.x = settingsBox_.x + 10;
	setting.settingTextBox.y = settingsBox_.y + settingNum * settingFontSize;
	setting.settingTextBox.w = settingsBox_.w;
	setting.settingTextBox.h = settingFontSize + settingOffset;
	setting.text = "ID: " + std::to_string(objectId_);
	setting.fontPath = "src/includes/fonts/Roboto/Roboto-Bold.ttf";
	setting.fontSize = settingFontSize;
	
	// Insert setting into package
	package.settings[settingNum - 1] = setting;
	settingNum++;

	// Create mass setting
	setting.settingTextBox.x = settingsBox_.x + 10;
	setting.settingTextBox.y = settingsBox_.y + settingNum * settingFontSize;
	setting.settingTextBox.w = settingsBox_.w;
	setting.settingTextBox.h = settingFontSize + settingOffset;
	setting.text = "Mass: " + std::to_string(mass_) + " kg";
	setting.fontPath = "src/includes/fonts/Roboto/Roboto-Regular.ttf";
	setting.fontSize = settingFontSize;

	// Insert setting into package
	package.settings[settingNum - 1] = setting;
	settingNum++;

	// Create radius setting
	setting.settingTextBox.x = settingsBox_.x + 10;
	setting.settingTextBox.y = settingsBox_.y + settingNum * settingFontSize;
	setting.settingTextBox.w = settingsBox_.w;
	setting.settingTextBox.h = settingFontSize + settingOffset;
	setting.text = "Radius: " + std::to_string(radius_) + " m";
	setting.fontPath = "src/includes/fonts/Roboto/Roboto-Regular.ttf";
	setting.fontSize = settingFontSize;
	
	// Insert setting into package
	package.settings[settingNum - 1] = setting;

	settingsBox_.x = this->GetX();
	settingsBox_.y = this->GetY();
	settingsBox_.w = 200;
	settingsBox_.h = settingNum * settingFontSize + 30;

	return package;
}

void PhysicsObject::SetLocation(const Vector2 location) {
	location_ = location;
}
void PhysicsObject::SetVelocity(const Vector2 velocity) {
	velocity_ = velocity;
}
void PhysicsObject::SetAcceleration(const Vector2 acceleration) {
	acceleration_ = acceleration;
}

void PhysicsObject::SetColor(const int r, const int g, const int b, const int a) {
	if (r >= 0) this->color_.r = r;
	if (g >= 0) this->color_.g = g;
	if (b >= 0) this->color_.b = b;
	if (a >= 0) this->color_.a = a;
}

void PhysicsObject::SetRadius(const int radius) {
	radius_ = radius;
}

void PhysicsObject::SetMass(const float mass) {
	mass_ = mass;
}

void PhysicsObject::ResetColor() {
	color_ = defaultColor_;
}

Universe::Universe(float* meters_per_pixel, int* origin_x, int* origin_y) {
	metersPerPixel_ = meters_per_pixel;
	originX_ = origin_x;
	originY_ = origin_y;
}

Universe::~Universe() {
	PhysicsObject* current = firstObject_;
	PhysicsObject* lastCurrent = firstObject_;

	while (current != nullptr) {
		lastCurrent = current;
		current = current->next;
		delete lastCurrent;
	}
}

void Universe::ClearUniverse() {
	PhysicsObject* current = firstObject_;
	PhysicsObject* prev = firstObject_;

	while (current != nullptr) {
		current = current->next;
		delete prev;
		prev = current;
	}
	firstObject_ = nullptr;
	lastObject_ = nullptr;
}


PhysicsObject* Universe::GetFirst() const {
	return firstObject_;
}
PhysicsObject* Universe::GetLast() const {
	return lastObject_;
}
PhysicsObject* Universe::GetObjectOnPosition(Vector2* location) const {
	if (firstObject_ == nullptr) {
		return nullptr;
	}

	// Temporary variables
	PhysicsObject* current = firstObject_;

	// Go through all objects
	while (current != nullptr) {

		Vector2 newLocation = *current->GetLocation();
		TransposePosition(&newLocation, *originX_, *originY_);
		ZoomPosition(&newLocation, *metersPerPixel_);
		TransposePosition(&newLocation, -*originX_, -*originY_);

		SDL_SetRenderDrawColor(Core::renderer_, 20, 20, 20, 255);
		SDL_RenderDrawLine(Core::renderer_, newLocation.x - 10, newLocation.y, newLocation.x + 10, newLocation.y);
		SDL_RenderDrawLine(Core::renderer_, newLocation.x, newLocation.y + 10, newLocation.x, newLocation.y - 10);

		const auto distanceBetween = static_cast<int>(DistanceDifference(&newLocation, location)); 
		if (distanceBetween <= current->GetRadius() / *metersPerPixel_) {
			return current;
		}

		current = current->next;
	}
	return nullptr;
}
PhysicsObject* Universe::GetObjectWithId(const int id) const {
	if (firstObject_ == nullptr) {
		return nullptr;
	}

	// Temporary variables
	PhysicsObject* current = firstObject_;

	// Go through all objects
	while (current != nullptr) {
		
		if (current->GetId() == id) {
			return current;
		}

		current = current->next;
	}
	return nullptr;
}

void Universe::InsertObject(PhysicsObject* object) {
	if (firstObject_ == nullptr) {
		firstObject_ = object;
		lastObject_ = object;
	}
	else {
		lastObject_->next = object;
		lastObject_ = object;
	}
}

PhysicsObject* Universe::SummonObject(SDL_Point* position, const double radius, const double mass, SDL_Color* color) {
	int newId;
	if (firstObject_ == nullptr) {
		newId = 0;
	} else {
		newId = lastObject_->GetId() + 1;
	}

	PhysicsObject* newObject = new PhysicsObject{newId, position, radius, mass, color};
	InsertObject(newObject);
	return newObject;
}