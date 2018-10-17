#include "Core.h"
#include "PhysicsEngine.h"
#include "TTF_FontDisplay.h"

SDL_Renderer* Core::renderer_;

Core::Core() {
	window_ = nullptr;
	running_ = true;
	pause_ = false;
	mouseX_ = 0;
	mouseY_ = 0;
}

int Core::OnExecute() {
	SDL_Event event;

	if (!OnInit()) {
		return -1;
	}

	while (running_) {
		while (SDL_PollEvent(&event)) {
			OnEvent(&event);
		}
		OnLoop();
		OnRender();
	}
	OnCleanUp();

	return 0;
}

bool Core::OnInit() {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return false;
	}

	window_ = SDL_CreateWindow("Physics Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth_,
	                           screenHeight_, SDL_WINDOW_OPENGL);

	if (window_ == nullptr) {
		printf("Could not create window %s\n", SDL_GetError());
		return false;
	}

	renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);

	pe_ = new PhysicsEngine(screenWidth_, screenHeight_);

	textDisplay_ = new FontDisplay;

	return true;
}

void Core::OnEvent(SDL_Event* event) {
	switch (event->type) {
	case SDL_QUIT:
		running_ = false;
		break;
	case SDL_MOUSEBUTTONDOWN:
		// Left button clicked
		if (event->button.button == SDL_BUTTON_LEFT) {
			// Check if hovering over obect
			SDL_GetMouseState(&mouseX_, &mouseY_);
			PhysicsObject* object = pe_->GetObjectOnPosition(&Vector2((float)mouseX_, (float)mouseY_));
			// If user selects the same object twice then unselect
			if (object == selectedObject_ && object != nullptr && selectedObjectAction_ == 1) {
				// Unselect the selectedObject
				selectedObject_ = nullptr;
				selectedObjectAction_ = 0;
			}
			else if (object == nullptr) {
				// If selectedObject has selection then mark position
				if (selectedObject_ != nullptr) {
					// Add force in the direction
					Vector2* pos1 = selectedObject_->GetLocation();
					Vector2 pos2(static_cast<float>(mouseX_), static_cast<float>(mouseY_));
					Vector2 dir = pos2 - *pos1;

					dir.setMag(0.002f);

					selectedObject_->ApplyForce(dir);
				}
				// Unselect the selectedObject
				selectedObject_ = nullptr;
				selectedObjectAction_ = 0;
			}
			else {
				selectedObject_ = object;
				selectedObjectAction_ = 1;
			}
		}
			// Right button clicked
		else if (event->button.button == SDL_BUTTON_RIGHT) {
			SDL_GetMouseState(&mouseX_, &mouseY_);

			PhysicsObject* object = pe_->GetObjectOnPosition(&Vector2((float)mouseX_, (float)mouseY_));
			if (object == selectedObject_ && object != nullptr && selectedObjectAction_ == 2) {
				// Unselect the selectedObject
				selectedObject_ = nullptr;
				selectedObjectAction_ = 0;
			}
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
				pe_->SummonObject(&position, 50, 100, &color);
			}
			else {
				selectedObject_ = object;
				selectedObjectAction_ = 2;
			}
		}
		break;
	case SDL_TEXTINPUT:
	case SDL_TEXTEDITING:
		if (selectedObjectAction_ == 2) {
			//std::cout << event->edit.text << std::endl;
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
		pe_->UpdatePhysics();
	}

	SDL_GetMouseState(&mouseX_, &mouseY_);

	PhysicsObject* object = pe_->GetObjectOnPosition(&Vector2((float)mouseX_, (float)mouseY_));
	// Mouse is hovering over an object
	if (object != nullptr) {
		object->SetColor(100, 20, 20);
	}

	pe_->UpdateGraphics();

	// Draw line between selected object and mouse
	if (selectedObject_ != nullptr && selectedObjectAction_ == 1) {
		SDL_RenderDrawLine(renderer_, static_cast<int>(selectedObject_->GetX()), static_cast<int>(selectedObject_->GetY()), mouseX_, mouseY_);
	}
	else if (selectedObject_ != nullptr && selectedObjectAction_ == 2) {
		SDL_Rect rect;
		rect.x = static_cast<int>(selectedObject_->GetX());
		rect.y = static_cast<int>(selectedObject_->GetY());
		rect.w = 200;
		rect.h = 200;
		SDL_SetRenderDrawColor(renderer_, 200, 200, 200, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(renderer_, &rect);
		SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);

		std::string message = "HELLO THERE";
		std::string font_path = "src/includes/fonts/Roboto/Roboto-Black.ttf";
		textDisplay_->CreateText({ rect.x + 10, rect.y + 10 }, { 100, 100 }, &message, &font_path, 20);
		textDisplay_->DisplayText();
	}
}

void Core::OnRender() const {
	if (!pause_) {
		SDL_RenderPresent(renderer_);

		SDL_SetRenderDrawColor(renderer_, 255, 255, 255, SDL_ALPHA_OPAQUE);

		SDL_RenderClear(renderer_);
	}
}

void Core::OnCleanUp() const {
	SDL_DestroyRenderer(renderer_);
	SDL_DestroyWindow(window_);
	SDL_Quit();
}

