#include "Core.h"
#include "PhysicsEngine.h"

Core::Core() {
	window = NULL;
	running = true;
	pause = false;
}

int Core::OnExecute() {
	SDL_Event event;

	if (OnInit() == false) {
		return -1;
	}

	while (running) {
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

	window = SDL_CreateWindow("Physics Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenWidth, screenHeight, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		printf("Could not create window %s\n", SDL_GetError());
		return 1;
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	pe = new PhysicsEngine(screenWidth, screenHeight);

	return true;
}

void Core::OnEvent(SDL_Event* event) {
	switch (event->type) {
		case SDL_QUIT:
			running = false;
			break;			
		case SDL_MOUSEBUTTONDOWN:
			// Left button clicked
			if (event->button.button == SDL_BUTTON_LEFT) {
				// Check if hovering over obect
				SDL_GetMouseState(&mouseX, &mouseY);
				PhysicsObject* object = pe->GetObjectOnPosition(&Vector2(mouseX, mouseY));
				// If user selects the same object twice then unselect
				if (object == selectedObject && object != nullptr && selectedObjectAction == 1) {
					// Unselect the selectedObject
					selectedObject = nullptr;
					selectedObjectAction = 0;
				}
				else if (object == nullptr) {
					// If selectedObject has selection then mark position
					if (selectedObject != nullptr) {
						// Add force in the direction
						Vector2* pos1 = selectedObject->getLocation();
						Vector2* pos2 = new Vector2(mouseX, mouseY);
						Vector2 dir = *pos2 - *pos1;

						dir.setMag(0.002);

						selectedObject->ApplyForce(dir);
					}
					// Unselect the selectedObject
					selectedObject = nullptr;
					selectedObjectAction = 0;
				}
				else {
					selectedObject = object;
					selectedObjectAction = 1;
				}
			}
			// Right button clicked
			else if (event->button.button == SDL_BUTTON_RIGHT) {
				SDL_GetMouseState(&mouseX, &mouseY);

				PhysicsObject* object = pe->GetObjectOnPosition(&Vector2(mouseX, mouseY));
				if (object == selectedObject && object != nullptr && selectedObjectAction == 2) {
					// Unselect the selectedObject
					selectedObject = nullptr;
					selectedObjectAction = 0;
				}
				else if (object == nullptr) {
					// Summon sphere
					SDL_Point position;
					position.x = mouseX;
					position.y = mouseY;
					SDL_Color color;
					color.r = 20;
					color.g = 20;
					color.b = 50;
					color.a = 255;
					pe->SummonObject(&position, 50, 100, &color);
				}
				else {
					selectedObject = object;
					selectedObjectAction = 2;
				}
			}
			break;
		case SDL_TEXTINPUT:
		case SDL_TEXTEDITING:
			if (selectedObjectAction == 2) {
				
				//std::cout << event->edit.text << std::endl;
			}
			break;
		case SDL_KEYDOWN:
			switch (event->key.keysym.sym) {
				case SDLK_SPACE:
					if (pause) pause = false;
					else pause = true;
					break;
				// Unselect all objects if escape or backspace is clicked
				case SDLK_ESCAPE:
				case SDLK_BACKSPACE:
					selectedObject = nullptr;
					selectedObjectAction = 0;
					break;
			}
			break;
	}
}

void Core::OnLoop() {
	TTF_Init();

	SDL_Color text_color = { 0, 150, 0, 255 };

	TTF_Font *font = TTF_OpenFont("src/includes/arial.ttf", 50);
	if (!font) {
		std::cout << "Failed to load font" << std::endl;
	}
	auto text_surface = TTF_RenderText_Solid(font, "hellö", text_color);
	if (!text_surface) {
		std::cout << "Failed to create text surface" << std::endl;
	}
	auto *text_texture = SDL_CreateTextureFromSurface(renderer, text_surface);
	if (!text_texture) {
		std::cout << "Failed to create text texture" << std::endl;
	}
	SDL_FreeSurface(text_surface);
	TTF_CloseFont(font);

	SDL_Rect rect;
	rect.x = 10;
	rect.y = 10;
	rect.w = 100;
	rect.h = 100;

	if (SDL_QueryTexture(text_texture, nullptr, nullptr, &rect.w, &rect.h) != 0) {
		std::cout << "QueryTexture not loading" << std::endl;
	}

	SDL_RenderCopy(renderer, text_texture, nullptr, &rect);
	// Delete the texture after display
	SDL_DestroyTexture(text_texture);

	if (!pause) {
		pe->UpdatePhysics();
	}

	SDL_GetMouseState(&mouseX, &mouseY);

	PhysicsObject* object = pe->GetObjectOnPosition(&Vector2(mouseX, mouseY));
	// Mouse is hovering over an object
	if (object != nullptr) {
		object->setColor(100, 20, 20);
	}

	pe->UpdateGraphics();

	// Draw line between selected object and mouse
	if (selectedObject != nullptr && selectedObjectAction == 1) {
		SDL_RenderDrawLine(renderer, selectedObject->getX(), selectedObject->getY(), mouseX, mouseY);
	}
	else if (selectedObject != nullptr && selectedObjectAction == 2) {
		SDL_Rect rect;
		rect.x = selectedObject->getX();
		rect.y = selectedObject->getY();
		rect.w = 200;
		rect.h = 200;
		SDL_SetRenderDrawColor(renderer, 200, 200, 200, SDL_ALPHA_OPAQUE);
		SDL_RenderFillRect(renderer, &rect);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
	}
}

void Core::OnRender() {
	if (!pause) {
		SDL_RenderPresent(renderer);

		SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

		SDL_RenderClear(renderer);
	}
}
void Core::OnCleanUp() {
	TTF_Quit();
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}