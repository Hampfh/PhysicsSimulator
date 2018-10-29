#include "Core.h"

SDL_Renderer* Core::renderer_;
PhysicsEngine* Core::pe_;

Core::Core() {
	window_ = nullptr;
	running_ = true;
	pause_ = false;
	mouseX_ = 0;
	mouseY_ = 0;
	originX_ = screenWidth_ / 2;
	originY_ = screenHeight_ / 2;
	fps_ = 60;
	renderCrossHair_ = false;
}

int Core::OnExecute() {
	SDL_Event event;

	if (!OnInit()) { return -1; }

	while (running_) {
		while (SDL_PollEvent(&event)) { OnEvent(&event); }
		OnLoop();
		OnRender();
	}
	OnCleanUp();
	return 0;
}

bool Core::OnInit() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) { return false; }

	window_ = SDL_CreateWindow("Physics Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth_,
	                           screenHeight_, SDL_WINDOW_OPENGL);

	if (window_ == nullptr) {
		printf("Could not create window %s\n", SDL_GetError());
		return false;
	}

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);

	// Enables blends and transparent objects
	SDL_SetRenderDrawBlendMode(renderer_, SDL_BLENDMODE_BLEND);

	pe_ = new PhysicsEngine;

	universe_ = new Universe(&originX_, &originY_, &zoom_);

	textDisplay_ = new FontDisplay;

	// Create console thread
	std::thread consoleInput(CheckConsole, universe_);
	consoleInput.detach();

	return true;
}

void Core::OnEvent(SDL_Event* event) {
	SDL_GetMouseState(&mouseX_, &mouseY_);

	hoverObject_ = universe_->GetObjectOnPosition(&Vector2((float)mouseX_, (float)mouseY_), zoom_);

	// Render cross hair on mouse
	SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
	SDL_RenderDrawLine(renderer_, mouseX_ - 10, mouseY_, mouseX_ + 10, mouseY_);
	SDL_RenderDrawLine(renderer_, mouseX_, mouseY_ + 10, mouseX_, mouseY_ - 10);

	switch (event->type) {
	case SDL_QUIT:
		running_ = false;
		break;
	case SDL_MOUSEBUTTONDOWN:
		// Left button clicked
		if (event->button.button == SDL_BUTTON_LEFT) {
			// If user selects the same object twice then unselect
			if (hoverObject_ == selectedObject_ && hoverObject_ != nullptr && simulatorState_ == 1) {
				EndState();
			}
			// An object was clicked
			else if (hoverObject_ != nullptr) {
				ChangeState(MOVEMENT);
			}
		}
		// Right button clicked
		else if (event->button.button == SDL_BUTTON_RIGHT) {
			if (hoverObject_ == selectedObject_ && hoverObject_ != nullptr && simulatorState_ == SHOW_PROPERTIES) {
				EndState();
			}
			// Summon a sphere if user didn't click an object
			else if (hoverObject_ == nullptr) {

				// Mass of earth (kg)
				//const double earth = 5.972 * pow(10, 24);
				//const double earthRadius = 6371*1000;

				const double earth = 1000;
				const double earthRadius = 10;

				// Summon sphere
				Vector2 position(mouseX_, mouseY_);

				SDL_Color color;
				color.r = 20;
				color.g = 20;
				color.b = 50;
				color.a = 255;
				universe_->SummonObject(&position, earthRadius, earth, &color);
			}
			else {
				// Assign object to setting view
				ChangeState(SHOW_PROPERTIES);
			}
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (hoverObject_ == nullptr && simulatorState_ == MOVEMENT) {
			// If selectedObject has selection then mark position
			if (selectedObject_ != nullptr) {
				// Add force in the direction
				Vector2* pos1 = selectedObject_->GetLocation();

				const Vector2 pos2(static_cast<float>(mouseX_), static_cast<float>(mouseY_));

				ApplyIndividualForce(selectedObject_, pos2, 0.1);
			}
		} 
		
		if (simulatorState_ == MOVEMENT) {
			EndState();
		}
		break;
	case SDL_MOUSEWHEEL:
		if (event->wheel.y < 0) {
			// zoom in
			originX_ = mouseX_;
			originY_ = mouseY_;
			zoom_ *= 0.9f;
		} else if (event->wheel.y > 0 ) {
			// zoom out
			originX_ = mouseX_;
			originY_ = mouseY_;
			zoom_ *= 1.1f;
		}
		break;
	case SDL_KEYDOWN:
		switch (event->key.keysym.sym) {
		case SDLK_SPACE:
			if (pause_) pause_ = false;
			else pause_ = true;
			break;
		// Unselect all objects if escape or backspace is clicked
		case SDLK_ESCAPE:
		case SDLK_BACKSPACE:
			EndState();
			pause_ = false;
			break;
		case SDLK_h:
			// halt the object, remove all velocity
			if (hoverObject_ != nullptr) { hoverObject_->GetVelocity()->SetMag(0); }
			break;
		case SDLK_UP:
			originY_ += 100;
			break;
		case SDLK_DOWN:
			originY_ += 100;
			break;
		case SDLK_c:
			universe_->ClearUniverse();
			break;
		case SDLK_x:
			renderCrossHair_ = !renderCrossHair_;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void Core::OnLoop() {
	if (!pause_) {
		pe_->UpdatePhysics(universe_->GetFirst(), optimalTime_, simulationSpeed_);
		// Action two can't exist in non paused
		//if (selectedObjectAction_ == 2) selectedObjectAction_ = 0;
	}
	else {
		// When pause draw pause logo
		DrawPauseLogo(screenWidth_ - 35, 10, {0, 0, 0, 255});
	}

	// Render dot on center of screen
	SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
	SDL_RenderDrawPoint(renderer_, screenWidth_ / 2, screenHeight_ / 2);

	UpdateGraphics();

	StabilizeFPS();
}

void Core::OnRender() {
	// Display everything on screen
	SDL_RenderPresent(renderer_);

	// Set draw color
	SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE);

	// Render background for next frame
	SDL_RenderClear(renderer_);
}

void Core::OnCleanUp() const {
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();

	delete pe_;
	delete universe_;
	delete textDisplay_;

}

void Core::ChangeState(const States new_state) {
	EndState();

	TextPackage package;

	switch(new_state) {
		case MOVEMENT: // Begin object movement state
			std::cout << "Started MOVEMENT state" << std::endl;
			selectedObject_ = hoverObject_;
		break;
		case SHOW_PROPERTIES: // Begin display object properites
			std::cout << "Started PROPERTIES state" << std::endl;
			selectedObject_ = hoverObject_;

			package = selectedObject_->PrepareObjectSettings();

			std::cout << "ADDED text textures" << std::endl;

			SDL_Color textColor;
			textColor.r = 20;
			textColor.g = 20;
			textColor.b = 20;
			textColor.a = 255;

			// Create text elements
			for (int i = 0; i < package.package_size; i++) {
				auto currentSetting = package.settings[i];
				std::cout << currentSetting.settingTextBox.x << std::endl;
				const auto settingObject = textDisplay_->CreateTextObject(
					currentSetting.settingTextBox, 
					&currentSetting.text, 
					&currentSetting.fontPath, 
					currentSetting.fontSize,
					textColor
				);
				std::cout << "Created object" << std::endl;
				if (i == 0) {
					tempSettingStorageFirst_ = settingObject;
					tempSettingStorageFirst_->mainRect = *package.settingsBox;
					std::cout << "FIRST OBJECT: " << tempSettingStorageFirst_->textRect.x << std::endl;
				} else if (i + 1 == package.package_size) {
					tempSettingStorageLast_ = settingObject;
				}
			}
		break;
		default:

		break;
	}
	simulatorState_ = new_state;
}

void Core::DrawPauseLogo(const int x, const int y, const SDL_Color color) {
	SDL_SetRenderDrawColor(renderer_, color.r, color.g, color.b, color.a);
	SDL_Rect rect;
	rect.x = x;
	rect.y = y;
	rect.w = 10;
	rect.h = 30;
	SDL_RenderFillRect(renderer_, &rect);
	rect.x = x - 15;
	rect.y = y;
	rect.w = 10;
	rect.h = 30;
	SDL_RenderFillRect(renderer_, &rect);
}

void Core::DrawSettingPackage() const {

	SDL_Rect newRect;
	newRect.x = selectedObject_->GetX();
	newRect.y = selectedObject_->GetY();
	newRect.w = tempSettingStorageFirst_->mainRect.w;
	newRect.h = tempSettingStorageFirst_->mainRect.h;

	// Convert position
	ConvertCoordinate(&newRect.x, originX_, zoom_);
	ConvertCoordinate(&newRect.y, originX_, zoom_);

	// Draw settings background
	SDL_SetRenderDrawColor(renderer_, 230, 230, 230, 225);
	SDL_RenderFillRect(renderer_, &newRect);

	FontDisplay::DisplayText(tempSettingStorageFirst_, tempSettingStorageLast_, &newRect);
}

void Core::EndState() {
	switch (simulatorState_) {
		case 1: // End selection state
			std::cout << "ENDED MOVEMENT state" << std::endl;
			selectedObject_ = nullptr;
		break;
		case 2: // End view properties state
			std::cout << "ENDED PROPERTIES state" << std::endl;
			selectedObject_ = nullptr;
			//pause_ = false;

			textDisplay_->DeleteTextObjects(tempSettingStorageFirst_, tempSettingStorageLast_);
			std::cout << "Removed text textures" << std::endl;
		break;
		default:
		break;
	}
	simulatorState_ = DEFAULT;
}



void Core::DrawCircle(Vector2 location, float radius, SDL_Color* color, const int cross_hair) const {

	radius = radius * zoom_;

	ConvertCoordinates(&location, originX_, originY_, zoom_);

	if (cross_hair) {
		// Render a x on the planet position
		SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
		SDL_RenderDrawLine(renderer_, location.x - 10, location.y, location.x + 10, location.y);
		SDL_RenderDrawLine(renderer_, location.x, location.y + 10, location.x, location.y - 10);	
	}

	for (auto dy = 1; dy <= radius; dy++) {
		const auto dx = floor(sqrt((2.0 * radius * dy) - (dy * dy)));
		SDL_SetRenderDrawColor(renderer_, color->r, color->g, color->b, color->a);
		SDL_RenderDrawLine(renderer_,
		    static_cast<int>(location.x - dx), 
			static_cast<int>(location.y + dy - radius),
			static_cast<int>(location.x + dx),
		    static_cast<int>(location.y + dy - radius)
		);
		
		SDL_RenderDrawLine(renderer_,
			static_cast<int>(location.x - dx), 
			static_cast<int>(location.y - dy + radius), 
			static_cast<int>(location.x + dx), 
			static_cast<int>(location.y - dy + radius)
		);
	}
}

void Core::StabilizeFPS() {
	// Calculate delta time
	optimalTime_ = static_cast<float>(1000) / static_cast<float>(fps_);

	SDL_Delay(optimalTime_);
}

void Core::UpdateGraphics() const {
	// Highlight hover object
	if (hoverObject_ != nullptr) { hoverObject_->SetColor(100, 20, 20); }

	PhysicsObject* current = universe_->GetFirst();
	while (current != nullptr) {
		DrawCircle(*current->GetLocation(), static_cast<int>(current->GetRadius()), current->GetColor(), renderCrossHair_);
		//current->DrawCircle(metersPerPixel_);
		current->ResetColor();
		current = current->next;
	}

	// Draw line between selected object and mouse
	if (selectedObject_ != nullptr && simulatorState_ == MOVEMENT) {
		Vector2 currentPos = *selectedObject_->GetLocation();

		ConvertCoordinates(&currentPos, originX_, originY_, zoom_);

		SDL_RenderDrawLine(renderer_, static_cast<int>(currentPos.x),
		                   static_cast<int>(currentPos.y), mouseX_, mouseY_);
	}
	// Display settings box
	else if (selectedObject_ != nullptr && simulatorState_ == SHOW_PROPERTIES) {
		DrawSettingPackage();
	}
}

void ConvertCoordinates(Vector2* position, const int origin_x, const int origin_y, const float zoom) {
	//InvertYAxis(position, screen_height);
	TransposePosition(position, origin_x, origin_y);
	ZoomPosition(position, zoom);
	TransposePosition(position, -origin_x, -origin_y);
}

void ConvertCoordinate(int* coordinate, const int origin, const float zoom) {
	TransposeCoordinate(coordinate, origin);
	ZoomCoordinate(coordinate, zoom);
	TransposeCoordinate(coordinate, -origin);
}

void TransposePosition(Vector2* position, const int origin_x, const int origin_y) {

	position->x = position->x - origin_x;
	position->y = position->y - origin_y;
}

void TransposeCoordinate(int* coordinate, const int origin) {
	*coordinate = *coordinate - origin;
}

void ZoomPosition(Vector2* position, const float zoom) {

	position->x = position->x * zoom;
	position->y = position->y * zoom;
}

void ZoomCoordinate(int* coordinate, const float zoom) {
	*coordinate = *coordinate * zoom;
}

void RenderLine(SDL_Renderer* renderer, const int x1, const int y1, const int x2, const int y2, const float zoom, const int origin_x, const int origin_y) {
	Vector2 position(static_cast<float>(x1), static_cast<float>(y1));
	Vector2 positionTwo(static_cast<float>(x2), static_cast<float>(y2));

	ConvertCoordinates(&position, origin_x, origin_y, zoom);
	ConvertCoordinates(&positionTwo, origin_x, origin_y, zoom);

	SDL_RenderDrawLine(renderer, static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(positionTwo.x), static_cast<int>(positionTwo.y));
}
