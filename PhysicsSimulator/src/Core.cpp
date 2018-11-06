#include "Core.h"
#include <windows.h>

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
	optimalTime_ = 0;
	zoomText_ = nullptr;
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

	// Create right bottom text
	SDL_Rect rect;
	rect.w = 150;
	rect.h = 20;
	rect.x = screenWidth_ - rect.w;
	rect.y = screenHeight_ - rect.h;

	SDL_Color color;
	color.r = 20;
	color.g = 20;
	color.b = 20;
	color.a = 255;

	std::string fontPath = "src/includes/fonts/Roboto/Roboto-Thin.ttf";
	std::string message = "Current zoom: " + std::to_string(zoom_);

	zoomText_ = textDisplay_->CreateTextObject(rect, &message, &fontPath, 12, color);

	// Create console thread
	std::thread consoleInput(RunInterpreter, universe_, &simulationSpeed_);
	consoleInput.detach();

	return true;
}

void Core::OnEvent(SDL_Event* event) {
	SDL_GetMouseState(&mouseX_, &mouseY_);

	hoverObject_ = universe_->GetObjectOnPosition(Vector2(static_cast<float>(mouseX_), static_cast<float>(mouseY_)), zoom_, screenWidth_, screenHeight_);

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
			if (hoverObject_ == selectedObject_ && hoverObject_ != nullptr && simulationStates_ & 1) {
				EndState(MOVEMENT);
			}
			// An object was clicked
			else if (hoverObject_ != nullptr) {
				AddState(MOVEMENT);
			}
		}
		// Right button clicked
		else if (event->button.button == SDL_BUTTON_RIGHT) {
			if (hoverObject_ == selectedObject_ && hoverObject_ != nullptr && simulationStates_ & SHOW_PROPERTIES) {
				EndState(SHOW_PROPERTIES);
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
				
				TransposePosition(&position, originX_, originY_);
				ZoomPosition(&position, 1 / zoom_);
				TransposePosition(&position, -originX_, -originY_);
				position.x += originX_ - static_cast<int>(screenWidth_ / 2);
				position.y += originY_ - static_cast<int>(screenHeight_ / 2);

				SDL_Color color;
				color.r = 20;
				color.g = 20;
				color.b = 50;
				color.a = 255;
				universe_->SummonObject(&position, earthRadius, earth, &color);
			}
			else {
				// Assign object to setting view
				AddState(SHOW_PROPERTIES);
			}
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (hoverObject_ == nullptr && simulationStates_ & MOVEMENT) {
			// If selectedObject has selection then mark position
			if (selectedObject_ != nullptr) {
				// Add force in the direction
				const Vector2 pos2(static_cast<float>(mouseX_), static_cast<float>(mouseY_));

				ApplyIndividualForce(selectedObject_, pos2, 1);
			}
		} 
		
		if (simulationStates_ & MOVEMENT) {
			EndState(MOVEMENT);
		}
		break;
	case SDL_MOUSEWHEEL:
		if (event->wheel.y < 0) {
			// zoom in
			if (simulationStates_ & LOCK_OBJECT) {
				
			} else {
				originX_ = mouseX_;
				originY_ = mouseY_;
			}
			zoom_ *= 0.9f;
			
			textDisplay_->DeleteTextObject(zoomText_);
			// Create right bottom text
			SDL_Rect rect;
			rect.w = 150;
			rect.h = 20;
			rect.x = screenWidth_ - rect.w;
			rect.y = screenHeight_ - rect.h;

			SDL_Color color;
			color.r = 20;
			color.g = 20;
			color.b = 20;
			color.a = 255;

			std::string fontPath = "src/includes/fonts/Roboto/Roboto-Thin.ttf";
			std::string message = "Current zoom: " + std::to_string(zoom_);

			zoomText_ = textDisplay_->CreateTextObject(rect, &message, &fontPath, 12, color);

		} else if (event->wheel.y > 0 ) {
			// zoom out
			if (simulationStates_ & LOCK_OBJECT) {
				
			} else {
				originX_ = mouseX_;
				originY_ = mouseY_;
			}
			zoom_ *= 1.1f;

			textDisplay_->DeleteTextObject(zoomText_);
			// Create right bottom text
			SDL_Rect rect;
			rect.w = 150;
			rect.h = 20;
			rect.x = screenWidth_ - rect.w;
			rect.y = screenHeight_ - rect.h;

			SDL_Color color;
			color.r = 20;
			color.g = 20;
			color.b = 20;
			color.a = 255;

			std::string fontPath = "src/includes/fonts/Roboto/Roboto-Thin.ttf";
			std::string message = "Current zoom: " + std::to_string(zoom_);

			zoomText_ = textDisplay_->CreateTextObject(rect, &message, &fontPath, 12, color);
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
			simulationStates_ = 0;
			pause_ = false;
			break;
		case SDLK_h:
			// halt the object, remove all velocity
			if (hoverObject_ != nullptr) { hoverObject_->GetVelocity()->Multiply(0); }
			break;
		case SDLK_UP:
			originY_ += 100;
			break;
		case SDLK_DOWN:
			originY_ -= 100;
			break;
		case SDLK_c:
			universe_->ClearUniverse();
			break;
		case SDLK_x:
			renderCrossHair_ = !renderCrossHair_;
			break;
		case SDLK_l:
			// Toggle lock object
			if (simulationStates_ & LOCK_OBJECT) {
				EndState(LOCK_OBJECT);
			} else {
				if (hoverObject_ != nullptr) {
					AddState(LOCK_OBJECT);
				}
			}
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
		pe_->UpdatePhysics(universe_, optimalTime_, simulationSpeed_);
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

	RunStates();

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

void Core::AddState(const States new_state) {
	TextPackage package;

	switch(new_state) {
		case MOVEMENT: // Begin object movement state
			selectedObject_ = hoverObject_;
		break;
		case SHOW_PROPERTIES: // Begin display object properties
			selectedObject_ = hoverObject_;

			package = selectedObject_->PrepareObjectSettings();

			SDL_Color textColor;
			textColor.r = 20;
			textColor.g = 20;
			textColor.b = 20;
			textColor.a = 255;

			// Create text elements
			for (int i = 0; i < package.package_size; i++) {
				auto currentSetting = package.settings[i];
				const auto settingObject = textDisplay_->CreateTextObject(
					currentSetting.settingTextBox, 
					&currentSetting.text, 
					&currentSetting.fontPath, 
					currentSetting.fontSize,
					textColor
				);
				if (i == 0) {
					tempSettingStorageFirst_ = settingObject;
					tempSettingStorageFirst_->mainRect = *package.settingsBox;
				} else if (i + 1 == package.package_size) {
					tempSettingStorageLast_ = settingObject;
				}
			}
		case LOCK_OBJECT:
			selectedObject_ = hoverObject_;
		break;
		default:

		break;
	}
	// Add state
	simulationStates_ = simulationStates_ | new_state;
}

void Core::RunStates() {
	// Remove LOCK_OBJECT state if selectedObject is nullptr
	if (simulationStates_ & LOCK_OBJECT && selectedObject_ == nullptr) {
		EndState(LOCK_OBJECT);
	}

	if (simulationStates_ & MOVEMENT) {
		Vector2 currentPos = *selectedObject_->GetLocation();

		ConvertCoordinates(&currentPos, originX_, originY_, zoom_, screenWidth_, screenHeight_);

		SDL_RenderDrawLine(renderer_, static_cast<int>(currentPos.x),
		                   static_cast<int>(currentPos.y), mouseX_, mouseY_);
	}
	if (simulationStates_ & SHOW_PROPERTIES) {
		DrawSettingPackage();
	}
	if (simulationStates_ & LOCK_OBJECT) {
		// Change origin position to selected objects position
		originX_ = selectedObject_->GetLocation()->x;
		originY_ = selectedObject_->GetLocation()->y;
	}
}

void Core::EndState(const States end_state) {
	switch (simulationStates_) {
		case 1: // End selection state
			selectedObject_ = nullptr;
		break;
		case 2: // End view properties state
			selectedObject_ = nullptr;
			//pause_ = false;

			textDisplay_->DeleteTextObjects(tempSettingStorageFirst_, tempSettingStorageLast_);
		break;
		case 3:
			// Code to execute when state three is ended
		break;
		default:
		break;
	}
	simulationStates_ = simulationStates_ & ~end_state;
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
	ConvertCoordinate(&newRect.x, originX_, zoom_, screenWidth_);
	ConvertCoordinate(&newRect.y, originY_, zoom_, screenHeight_);

	// Draw settings background
	SDL_SetRenderDrawColor(renderer_, 230, 230, 230, 225);
	SDL_RenderFillRect(renderer_, &newRect);

	FontDisplay::DisplayText(tempSettingStorageFirst_, tempSettingStorageLast_, &newRect);
}

void Core::DrawCircle(Vector2 location, float radius, SDL_Color* color, const int cross_hair) const {

	radius = radius * zoom_;

	// Optimizations (never draw a circle bigger than screen)
	if (radius > screenWidth_) {
		radius = screenWidth_;
	}

	ConvertCoordinates(&location, originX_, originY_, zoom_, screenWidth_, screenHeight_);

	if (cross_hair) {
		// Render a x on the planet position
		SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
		SDL_RenderDrawLine(renderer_, location.x - 10, location.y, location.x + 10, location.y);
		SDL_RenderDrawLine(renderer_, location.x, location.y + 10, location.x, location.y - 10);	
	}

	for (auto dy = 1; dy <= radius + 1; dy++) {
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
		current->ResetColor();
		current = current->next;
	}
	
	FontDisplay::DisplayText(zoomText_);

	// Render cross hair att origin
	SDL_SetRenderDrawColor(renderer_, 20, 20, 20, 255);
	SDL_RenderDrawLine(renderer_, originX_ - 10, originY_, originX_ + 10, originY_);
	SDL_RenderDrawLine(renderer_, originX_, originY_ + 10, originX_, originY_ - 10);	
}

void ConvertCoordinates(Vector2* position, const int origin_x, const int origin_y, const float zoom, const int screen_width, const int screen_height) {
	CenterOrigin(position, origin_x, origin_y, screen_width, screen_height);
	TransposePosition(position, origin_x, origin_y);
	ZoomPosition(position, zoom);
	TransposePosition(position, -origin_x, -origin_y);
}

void ConvertCoordinate(int* coordinate, const int origin, const float zoom, const int screen) {
	CenterCoordinate(coordinate, origin, screen);
	TransposeCoordinate(coordinate, origin);
	ZoomCoordinate(coordinate, zoom);
	TransposeCoordinate(coordinate, -origin);
}

void CenterOrigin(Vector2* position, const int origin_x, const int origin_y, const int screen_width, const int screen_height) {
	position->x -= origin_x - static_cast<int>(screen_width / 2);
	position->y -= origin_y - static_cast<int>(screen_height / 2);
}

void ReverseOrigin(Vector2* position, const int origin_x, const int origin_y, const int screen_width, const int screen_height) {
	position->x += origin_x - static_cast<int>(screen_width / 2);
	position->y += origin_y - static_cast<int>(screen_height / 2);
}


void CenterCoordinate(int* coordinate, const int origin, const int screen) {
	*coordinate -= origin - static_cast<int>(screen / 2);
}

void ReverseCoordinate(int* coordinate, const int origin, const int screen) {
	*coordinate -= origin - static_cast<int>(screen / 2);
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
