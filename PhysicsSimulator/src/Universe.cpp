#include "Universe.h"

PhysicsObject::PhysicsObject(const int id, SDL_Point* position, const int radius, const float mass, SDL_Color* color)
: objectId_(id), radius_(radius), mass_(mass), color_(*color) {
	location_.Set(static_cast<float>(position->x), static_cast<float>(position->y));
	velocity_.Set(0,0);
	acceleration_.Set(0, 0);
	defaultColor_ = color_;
}

void PhysicsObject::ApplyForce(const Vector2 force) {
	acceleration_ = acceleration_ + force;
}

void PhysicsObject::DrawCircle() const {
	for (double dy = 1; dy <= radius_; dy += 1) {
		const double dx = floor(sqrt((2.0 * radius_ * dy) - (dy * dy)));
		SDL_SetRenderDrawColor(Core::renderer_, color_.r, color_.g, color_.b, color_.a);
		SDL_RenderDrawLine(Core::renderer_,
		    static_cast<int>(location_.x() - dx), 
			static_cast<int>(location_.y() + dy - radius_),
			static_cast<int>(location_.x() + dx),
		    static_cast<int>(location_.y() + dy - radius_)
		);
		
		SDL_RenderDrawLine(Core::renderer_, 
			static_cast<int>(location_.x() - dx), 
			static_cast<int>(location_.y() - dy + radius_), 
			static_cast<int>(location_.x() + dx), 
			static_cast<int>(location_.y() - dy + radius_)
		);
	}
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

	// Settings container
	settingsBox_.x = this->GetX();
	settingsBox_.y = this->GetY();
	settingsBox_.w = 200;
	settingsBox_.h = settingNum * settingFontSize + 30;

	return package;
}

// Updates the single object
void PhysicsObject::Update() {
	velocity_ = velocity_ + acceleration_;
	location_ = location_ + velocity_;
	acceleration_.setMag(0);
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

Universe::~Universe() {
	PhysicsObject* current = firstObject_;
	PhysicsObject* lastCurrent = firstObject_;

	while (current != nullptr) {
		lastCurrent = current;
		current = current->next;
		delete lastCurrent;
	}
}

PhysicsObject* Universe::GetFirst() {
	return firstObject_;
}
PhysicsObject* Universe::GetLast() {
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
		
		const int distanceBetween = static_cast<int>(DistanceDifference(current->GetLocation(), location)); 
		if (distanceBetween <= current->GetRadius()) {
			return current;
		}

		current = current->next;
	}
	return nullptr;
}
PhysicsObject* Universe::GetObjectWithId(int id) const {
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

PhysicsObject* Universe::SummonObject(SDL_Point* position, int radius, int mass, SDL_Color* color) {
	int new_id;
	if (firstObject_ == nullptr) {
		new_id = 0;
	} else {
		new_id = lastObject_->GetId() + 1;
	}

	PhysicsObject* const newObject = new PhysicsObject{new_id, position, radius, static_cast<float>(mass), color};
	InsertObject(newObject);
	return newObject;
}