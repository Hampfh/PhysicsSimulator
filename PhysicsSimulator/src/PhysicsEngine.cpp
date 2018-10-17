#include "PhysicsEngine.h"
#include <string>

////////////////////////////////////////////////// PhysicsObject //////////////////////////////////////////////////////////////

PhysicsObject::PhysicsObject(const int id, SDL_Point* position, const int radius, const float mass, SDL_Color* color)
: objectId_(id), radius_(radius), mass_(mass), color_(*color) {
	location_.Set(static_cast<float>(position->x), static_cast<float>(position->y));
	velocity_.Set(0,0);
	acceleration_.Set(0, 0);
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

	int setting_num = 1;
	const int setting_offset = 5;
	const int setting_font_size = 15;

	TextString setting;

	// Create ID
	setting.settingTextBox.x = settingsBox_.x + 10;
	setting.settingTextBox.y = settingsBox_.y + setting_num * setting_font_size;
	setting.settingTextBox.w = settingsBox_.w;
	setting.settingTextBox.h = setting_font_size + setting_offset;
	setting.text = "ID: " + std::to_string(objectId_);
	setting.fontPath = "src/includes/fonts/Roboto/Roboto-Bold.ttf";
	setting.fontSize = setting_font_size;
	
	// Insert setting into package
	package.settings[setting_num - 1] = setting;
	setting_num++;

	// Create mass setting
	setting.settingTextBox.x = settingsBox_.x + 10;
	setting.settingTextBox.y = settingsBox_.y + setting_num * setting_font_size;
	setting.settingTextBox.w = settingsBox_.w;
	setting.settingTextBox.h = setting_font_size + setting_offset;
	setting.text = "Mass: " + std::to_string(mass_);
	setting.fontPath = "src/includes/fonts/Roboto/Roboto-Regular.ttf";
	setting.fontSize = setting_font_size;

	// Insert setting into package
	package.settings[setting_num - 1] = setting;
	setting_num++;

	// Create radius setting
	setting.settingTextBox.x = settingsBox_.x + 10;
	setting.settingTextBox.y = settingsBox_.y + setting_num * setting_font_size;
	setting.settingTextBox.w = settingsBox_.w;
	setting.settingTextBox.h = setting_font_size + setting_offset;
	setting.text = "Radius: " + std::to_string(radius_);
	setting.fontPath = "src/includes/fonts/Roboto/Roboto-Regular.ttf";
	setting.fontSize = setting_font_size;
	
	// Insert setting into package
	package.settings[setting_num - 1] = setting;

	// Settings container
	settingsBox_.x = this->GetX();
	settingsBox_.y = this->GetY();
	settingsBox_.w = 200;
	settingsBox_.h = setting_num * setting_font_size + 30;

	return package;
}

// Updates the single object
void PhysicsObject::Update() {
	velocity_ = velocity_ + acceleration_;
	location_ = location_ + velocity_;
	// Reset color 
	color_.r = 20;
	color_.g = 20;
	color_.b = 20;
	acceleration_.setMag(0);
}

void PhysicsObject::SetColor(int r, int g, int b, int a) {
	if (r >= 0) this->color_.r = r;
	if (g >= 0) this->color_.g = g;
	if (b >= 0) this->color_.b = b;
	if (a >= 0) this->color_.a = a;
}

////////////////////////////////////////////////// PhysicsEngine //////////////////////////////////////////////////////////////

PhysicsEngine::PhysicsEngine(const int simulation_width, const int simulation_height) 
	: simulationWidth_(simulation_width), simulationHeight_(simulation_height) {

}


PhysicsEngine::~PhysicsEngine() {
	// Delete queue
	PhysicsObject* current = firstObject_;
	while (current != nullptr) {
		current = current->next;
		delete firstObject_;
		firstObject_ = current;
	}
}

void PhysicsEngine::UpdatePhysics() {
	// Temporary variables
	PhysicsObject* current = firstObject_;
	PhysicsObject* current_matcher = firstObject_;

	// Go through all objects
	while (current != nullptr) {
		current_matcher = firstObject_;
		while (current_matcher != nullptr) {
			if (current_matcher == current) {
				current_matcher = current_matcher->next;
				continue; // Continues if matches with itself
			}

			Vector2* pos1 = current->GetLocation();
			Vector2* pos2 = current_matcher->GetLocation();
			Vector2 dir = *pos2 - *pos1;

			/* Newtons law of gravity
				  m1 * m2
			F = G --------
				    r^2
			*/
			const float force_strength = static_cast<float>(6.672f * pow(10, -11)) * ((current->GetMass() * current_matcher->GetMass()) / pow(DistanceDifference(current, current_matcher), 2));

			dir.setMag(force_strength * 100000);

			current->ApplyForce(dir);

			current_matcher = current_matcher->next;
		}
		current->Update();
		current = current->next;
	}

	SDL_Delay(1);
}

void PhysicsEngine::UpdateGraphics() {
	PhysicsObject* current = firstObject_;
	while (current != nullptr) {
		current->DrawCircle();
		current = current->next;
	}
}

void PhysicsEngine::AddToQueue(PhysicsObject* object) {
	if (firstObject_ == nullptr) {
		firstObject_ = object;
		lastObject_ = object;
	}
	else {
		lastObject_->next = object;
		lastObject_ = object;
	}
}

Vector2 PhysicsEngine::SDLPointToVec2(SDL_Point* point) {
	// Braced initialization
	return Vector2{static_cast<float>(point->x), static_cast<float>(point->y)};
}

float PhysicsEngine::DistanceDifference(PhysicsObject* point, PhysicsObject* point_two) {
	const float difference_x = abs(point->GetX() - point_two->GetX());
	const float difference_y = abs(point->GetY() - point_two->GetY());

	return sqrt(pow(difference_x, 2) + pow(difference_y, 2));
}
float PhysicsEngine::DistanceDifference(Vector2* point, Vector2* point_two) {
	const float difference_x = abs(point->x() - point_two->x());
	const float difference_y = abs(point->y() - point_two->y());

	return sqrt(pow(difference_x, 2) + pow(difference_y, 2));
}

PhysicsObject* PhysicsEngine::SummonObject(SDL_Point* position, const int radius, const int mass, SDL_Color* color) {
	int new_id;
	if (firstObject_ == nullptr) {
		new_id = 0;
	} else {
		new_id = lastObject_->GetId() + 1;
	}

	PhysicsObject* const new_object = new PhysicsObject{new_id, position, radius, static_cast<float>(mass), color};
	AddToQueue(new_object);
	return new_object;
}

PhysicsObject* PhysicsEngine::GetObjectOnPosition(Vector2* location) {
	// Temporary variables
	PhysicsObject* current = firstObject_;

	// Go through all objects
	while (current != nullptr) {
		
		const int distance_between = static_cast<int>(DistanceDifference(current->GetLocation(), location)); 
		if (distance_between <= current->GetRadius()) {
			return current;
		}

		current = current->next;
	}
	return nullptr;
}
