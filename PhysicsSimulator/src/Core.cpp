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
	updateFreq_ = 0;
	fps_ = 60;
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

	pe_ = new PhysicsEngine;

	universe_ = new Universe(&zoom_, &originX_, &originY_);

	textDisplay_ = new FontDisplay;

	// Create console thread
	std::thread consoleInput(&Core::CheckConsole, Core(), universe_);
	consoleInput.detach();

	return true;
}

void Core::OnEvent(SDL_Event* event) {
	SDL_GetMouseState(&mouseX_, &mouseY_);

	hoverObject_ = universe_->GetObjectOnPosition(&Vector2((float)mouseX_, (float)mouseY_));

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
			if (hoverObject_ == selectedObject_ && hoverObject_ != nullptr && selectedObjectAction_ == 1) {
				// Unselect the selectedObject
				selectedObject_ = nullptr;
				selectedObjectAction_ = 0;
			}
			// An object was clicked
			else if (hoverObject_ != nullptr) {
				selectedObject_ = hoverObject_;
				selectedObjectAction_ = 1;
			}
		}
		// Right button clicked
		else if (event->button.button == SDL_BUTTON_RIGHT) {
			if (hoverObject_ == selectedObject_ && hoverObject_ != nullptr && selectedObjectAction_ == 2) {
				// Unselect the selectedObject
				selectedObject_ = nullptr;
				selectedObjectAction_ = 0;
				pause_ = false;
			}
			// Summon a sphere if user didn't click an object
			else if (hoverObject_ == nullptr) {

				// Mass of earth (kg)
				//const double earth = 5.972 * pow(10, 24);
				//const double earthRadius = 6371*1000;

				std::cout << "Zoom: " << zoom_ << std::endl;
				std::cout << "Mousex: " << mouseX_ << " Mousey: " << mouseY_ << std::endl;

				const double earth = 10000;
				const double earthRadius = 10;

				// Summon sphere
				Vector2 position(mouseX_, mouseY_);
				ConvertCoordinates(&position, originX_, originY_, zoom_);

				SDL_Point pointPosition;
				pointPosition.x = position.x;
				pointPosition.y = position.y;
				//pointPosition.x = mouseX_;
				//pointPosition.y = mouseY_;

				std::cout << "Transposed X: " << pointPosition.x << " : Transposed Y: " << pointPosition.y << std::endl;

				SDL_Color color;
				color.r = 20;
				color.g = 20;
				color.b = 50;
				color.a = 255;
				universe_->SummonObject(&pointPosition, earthRadius, earth, &color);
			}
			else {
				// Assign object to setting view
				selectedObject_ = hoverObject_;
				selectedObjectAction_ = 2;
				pause_ = true;
			}
		}
		break;
	case SDL_MOUSEBUTTONUP:
		if (hoverObject_ == nullptr && selectedObjectAction_ == 1) {
			// If selectedObject has selection then mark position
			if (selectedObject_ != nullptr) {
				// Add force in the direction
				Vector2* pos1 = selectedObject_->GetLocation();
				const Vector2 pos2(static_cast<float>(mouseX_), static_cast<float>(mouseY_));
				Vector2 dir = pos2 - *pos1;

				dir.SetMag(0.0001f / timeInterval_);

				selectedObject_->ApplyForce(dir);
			}
			// Unselect the selectedObject
			selectedObject_ = nullptr;
			selectedObjectAction_ = 0;
		} else if (selectedObjectAction_ == 1) {
			// Unselect the selectedObject
			selectedObject_ = nullptr;
			selectedObjectAction_ = 0;
		}
		break;
	case SDL_MOUSEWHEEL:
		if (event->wheel.y < 0) {
			// Scroll against
			//originX_ = mouseX_;
			//originY_ = mouseY_;
			zoom_ /= 1.1f;
			
		} else if (event->wheel.y > 0 ) {
			// Scroll away
			//originX_ = mouseX_;
			//originY_ = mouseY_;
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
			selectedObject_ = nullptr;
			selectedObjectAction_ = 0;
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
		default:
			break;
		}
		break;
	default:
		break;
	}
}

void Core::OnLoop() {

	std::cout << "FPS: " << fps_ << " Frequency: " << static_cast<float>(updateFreq_) / static_cast<float>(1000) << std::endl;

	if (!pause_) {
		pe_->UpdatePhysics(universe_->GetFirst(), timeInterval_);
		// Action two can't exist in non paused
		if (selectedObjectAction_ == 2) selectedObjectAction_ = 0;
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
	SDL_RenderPresent(renderer_);

	SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE);

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

void Core::CheckConsole(Universe* universe) const {
	while (true) {
		std::string input;
		std::getline(std::cin, input);
		ConsoleInterpretation(&input, universe);	
	}
}

void Core::ConsoleInterpretation(std::string* command, Universe* universe) const {
	const int commandMaxArguments = 4;
	std::string input[commandMaxArguments];
	std::stringstream ss(*command);
	std::string tok;

	int i = 0;
	while (std::getline(ss, tok, ' ')) {
		if (i > commandMaxArguments) { break; }
		input[i] = tok;
		i++;
	}

	// Commands
	if (input[0] == "/target") {
		if (IsNumber(input[1])) {
			const int id = std::stoi(input[1]);
			auto object = universe->GetObjectWithId(id);

			if (object == nullptr) {
				std::cout << "ERROR: No object with ID:" << id << " found" << std::endl;
				return;
			}

			if (input[2] == "setMass") {
				if (IsNumber(input[3])) {
					const float value = std::stof(input[3]);

					// Perform request
					object->SetMass(value);
					std::cout << "Mass successfully changed to " << value << std::endl;
				}
				else { std::cout << "ERROR: No number was entered" << std::endl; }
			}
			else if (input[2] == "setRadius") {
				if (IsNumber(input[3])) {
					const int value = std::stoi(input[3]);

					// Perform request
					object->SetRadius(value);
					std::cout << "Radius successfully changed to " << value << std::endl;
				}
				else { std::cout << "ERROR: No number was entered" << std::endl; }
			}
			else if (input[2].empty()) { std::cout << "System: Found object <" << object << ">" << std::endl; }
			else { std::cout << "ERROR: No such command" << std::endl; }
		}
		else { std::cout << "ERROR: Input was not number" << std::endl; }
	} else if (input[0] == "/toggle") {
		if (input[1] == "forces") {
			std::cout << "SETTING OFF" << std::endl;
		} else if (input[1] == "velocity") {
			std::cout << "SETTING OFF" << std::endl;
		} else {
			std::cout << "ERROR: No specifier" << std::endl;
		}
	} else {
		std::cout << "ERROR: No such command" << std::endl;
	}
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

void Core::DrawSettingPackage(TextPackage* package) const {
	SDL_SetRenderDrawColor(renderer_, 230, 230, 230, SDL_ALPHA_OPAQUE);
	Vector2 position(package->settingsBox->x, package->settingsBox->y);
	ConvertCoordinates(&position, originX_, originY_, zoom_);
	package->settingsBox->x = position.x;
	package->settingsBox->y = position.y;
	SDL_RenderFillRect(renderer_, package->settingsBox);

	for (int i = 0; i < package->package_size; i++) {
		auto current_setting = package->settings[i];
		textDisplay_->CreateText(
			current_setting.settingTextBox,
			&current_setting.text,
			&current_setting.fontPath,
			current_setting.fontSize
		);
		textDisplay_->DisplayText();
	}
}

bool Core::IsNumber(const std::string& s) {
	// Checks the first character and the last character
	return (!(s.empty()) && isdigit(s[0]));
}

void Core::DrawCircle(Vector2 location, int radius, SDL_Color* color) const {

	radius = static_cast<int>(static_cast<float>(radius) * zoom_);

	TransposePosition(&location, originX_, originY_);
	ZoomPosition(&location, zoom_);
	TransposePosition(&location, -originX_, -originY_);

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
	updateFreq_ = 1000 / fps_;

	currentTime_ = SDL_GetTicks();

	int sleepTime = (updateFreq_ - (currentTime_ - lastUpdated_));

	if (sleepTime < 1) {
		sleepTime = 1;
	} else if (sleepTime > updateFreq_) {
		sleepTime = updateFreq_;
	}

	SDL_Delay(sleepTime);

	lastUpdated_ = currentTime_;
}

void Core::UpdateGraphics() const {
	// Mouse is hovering over an object
	if (hoverObject_ != nullptr) { hoverObject_->SetColor(100, 20, 20); }

	PhysicsObject* current = universe_->GetFirst();
	while (current != nullptr) {
		DrawCircle(*current->GetLocation(), static_cast<int>(current->GetRadius()), current->GetColor());
		//current->DrawCircle(metersPerPixel_);
		current->ResetColor();
		current = current->next;
	}

	// Draw line between selected object and mouse
	if (selectedObject_ != nullptr && selectedObjectAction_ == 1) {
		Vector2 currentPos = *selectedObject_->GetLocation();

		ConvertCoordinates(&currentPos, originX_, originY_, zoom_);

		SDL_RenderDrawLine(renderer_, static_cast<int>(currentPos.x),
		                   static_cast<int>(currentPos.y), mouseX_, mouseY_);
	}
	// Display settings box
	else if (selectedObject_ != nullptr && selectedObjectAction_ == 2) {
		TextPackage package = selectedObject_->PrepareObjectSettings();
		DrawSettingPackage(&package);
	}
}

void ConvertCoordinates(Vector2* position, const int origin_x, const int origin_y, const float zoom) {
	//InvertYAxis(position, screen_height);
	TransposePosition(position, origin_x, origin_y);
	ZoomPosition(position, zoom);
	TransposePosition(position, -origin_x / zoom, -origin_y / zoom);
}

void InvertYAxis(Vector2* position, const int screen_height) {
	position->y = screen_height - position->y;
}

void TransposePosition(Vector2* position, const int origin_x, const int origin_y) {

	position->x = position->x - origin_x;
	position->y = position->y - origin_y;
}

void ZoomPosition(Vector2* position, const float zoom) {

	position->x = position->x * zoom;
	position->y = position->y * zoom;
}

void RenderLine(SDL_Renderer* renderer, const int x1, const int y1, const int x2, const int y2, const float zoom, const int origin_x, const int origin_y) {
	Vector2 position(static_cast<float>(x1), static_cast<float>(y1));
	Vector2 positionTwo(static_cast<float>(x2), static_cast<float>(y2));

	ConvertCoordinates(&position, origin_x, origin_y, zoom);
	ConvertCoordinates(&positionTwo, origin_x, origin_y, zoom);

	SDL_RenderDrawLine(renderer, static_cast<int>(position.x), static_cast<int>(position.y), static_cast<int>(positionTwo.x), static_cast<int>(positionTwo.y));
}
