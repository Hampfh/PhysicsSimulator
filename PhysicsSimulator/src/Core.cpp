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
			// Left button
			if (event->button.button == SDL_BUTTON_LEFT) {
				// Check if hovering over obect
				int mouseX, mouseY;

				SDL_GetMouseState(&mouseX, &mouseY);
				PhysicsObject* object = pe->GetObjectOnPosition(&Vector2(mouseX, mouseY));
				if (object == nullptr) {
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
					break;
				}
				selectedObject = object;
			}
			// Right button clicked, summon a sphere
			// == SUMMON A CIRCLE ON CLICK ==
			else if (event->button.button == SDL_BUTTON_RIGHT) {
				int x;
				int y;
				SDL_GetMouseState(&x, &y);

				SDL_Point position;
				position.x = x;
				position.y = y;
				SDL_Color color;
				color.r = 20;
				color.g = 20;
				color.b = 50;
				color.a = 255;
				pe->SummonObject(&position, 50, 100, &color);
			}
			break;
		case SDL_KEYDOWN:
			switch (event->key.keysym.sym) {
				case SDLK_SPACE:
					if (pause) pause = false;
					else pause = true;
					break;
			}
			break;
	}
}

void Core::OnLoop() {

	if (!pause) {
		pe->UpdatePhysics();
	}

	int mouseX, mouseY;
	SDL_GetMouseState(&mouseX, &mouseY);

	PhysicsObject* object = pe->GetObjectOnPosition(&Vector2(mouseX, mouseY));
	// Mouse is hovering over an object
	if (object != nullptr) {
		object->setColor(100, 20, 20);
	}

	pe->UpdateGraphics();

	// Draw line between selected object and mouse
	if (selectedObject != nullptr) {
		SDL_RenderDrawLine(renderer, selectedObject->getX(), selectedObject->getY(), mouseX, mouseY);
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
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}