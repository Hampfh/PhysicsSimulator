#include "Core.h"

SDL_Renderer* Core::renderer_;
PhysicsEngine* Core::pe_;

Core::Core() {
	window_ = nullptr;
	running_ = true;
	pause_ = false;
	mouseX_ = 0;
	mouseY_ = 0;
	timeInterval_ = 0.001;
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

	universe_ = new Universe;

	textDisplay_ = new FontDisplay;

	// Create console thread
	std::thread console_input(&Core::CheckConsole, Core());
	console_input.detach();

	return true;
}

void Core::OnEvent(SDL_Event* event) {
	SDL_GetMouseState(&mouseX_, &mouseY_);
	hoverObject_ = universe_->GetObjectOnPosition(&Vector2((float)mouseX_, (float)mouseY_));

	switch (event->type) {
	case SDL_QUIT:
		running_ = false;
		break;
	case SDL_MOUSEBUTTONDOWN:

		// If click was performed outside screen then ignore
		if (mouseX_ < 0 || mouseX_ > screenWidth_ ||
			mouseY_ < 0 || mouseY_ > screenHeight_) { return; }

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
			PhysicsObject* object = universe_->GetObjectOnPosition(&Vector2((float)mouseX_, (float)mouseY_));
			if (object == selectedObject_ && object != nullptr && selectedObjectAction_ == 2) {
				// Unselect the selectedObject
				selectedObject_ = nullptr;
				selectedObjectAction_ = 0;
				pause_ = false;
			}
				// Summon a sphere if user didn't click an object
			else if (object == nullptr) {
				// Summon sphere
				SDL_Point position;
				position.x = mouseX_;
				position.y = mouseY_;
				SDL_Color color;
				color.r = 20;
				color.g = 20;
				color.b = 50;
				color.a = 255;
				universe_->SummonObject(&position, 50, 100, &color);
			}
			else {
				// Assign object to setting view
				selectedObject_ = object;
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
				Vector2 pos2(static_cast<float>(mouseX_), static_cast<float>(mouseY_));
				Vector2 dir = pos2 - *pos1;

				dir.setMag(0.001f);

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
			
		} else if (event->wheel.y > 0 ){
			// Scroll away
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
			if (hoverObject_ != nullptr) { hoverObject_->GetVelocity()->setMag(0); }
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
		pe_->UpdatePhysics(universe_->GetFirst(), timeInterval_);
		// Action two can't exist in non paused
		if (selectedObjectAction_ == 2) selectedObjectAction_ = 0;
	}
	else {
		// When pause draw pause logo
		DrawPauseLogo(screenWidth_ - 35, 10, {0, 0, 0, 255});
	}

	UpdateGraphics();

	// From seconds to milliseconds
	SDL_Delay(timeInterval_ * 1000);
	std::cout << timeInterval_ * 1000 << std::endl;
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
	std::cout << "CLEAN UP" << std::endl;
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

void Core::CheckConsole() const {
	std::string input;
	std::getline(std::cin, input);
	ConsoleInterpretation(&input);
	CheckConsole();
}

void Core::ConsoleInterpretation(std::string* command) const {
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
			auto object = universe_->GetObjectWithId(id);

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

bool Core::IsNumber(const std::string& s) {
	// Checks the first character and the last character
	return (!(s.empty()) && isdigit(s[0]));
}

void Core::UpdateGraphics() const {
	// Mouse is hovering over an object
	if (hoverObject_ != nullptr) { hoverObject_->SetColor(100, 20, 20); }

	PhysicsObject* current = universe_->GetFirst();
	while (current != nullptr) {
		current->DrawCircle();
		current->ResetColor();
		current = current->next;
	}

	// Draw line between selected object and mouse
	if (selectedObject_ != nullptr && selectedObjectAction_ == 1) {
		SDL_RenderDrawLine(renderer_, static_cast<int>(selectedObject_->GetX()),
		                   static_cast<int>(selectedObject_->GetY()), mouseX_, mouseY_);
	}
	// Display settings box
	else if (selectedObject_ != nullptr && selectedObjectAction_ == 2) {
		TextPackage package = selectedObject_->PrepareObjectSettings();
		DrawSettingPackage(&package);
	}
}
