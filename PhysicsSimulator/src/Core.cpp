#include "Core.h"
#include "PhysicsEngine.h"

Core::Core() {
	window = NULL;
	running = true;
}

int Core::OnExecute() {
	if (OnInit() == false) {
		return -1;
	}
	SDL_Event event;

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

	pe = new PhysicsEngine;

	// Properies for object
	SDL_Point position;
	position.x = screenWidth/2;
	position.y = screenHeight / 2;
	SDL_Color color;
	color.r = 20;
	color.g = 20;
	color.b = 20;
	color.a = 255;
	pe->SummonObject(&position, 50, 0, &color);

	return true;
}

void Core::OnEvent(SDL_Event* event) {
	if (event->type == SDL_QUIT) {
		running = false;
	}
}

void Core::OnLoop() {
	pe->UpdatePhysics();
}

void Core::OnRender() {

	SDL_RenderPresent(renderer);

	SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);

	SDL_RenderClear(renderer);
}
void Core::OnCleanUp() {
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}