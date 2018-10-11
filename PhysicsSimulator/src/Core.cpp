#include "Core.h"

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
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		return false;
	}

	window = SDL_CreateWindow("Physics Simulator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 680, 400, SDL_WINDOW_OPENGL);

	if (window == NULL) {
		printf("Could not create window %s\n", SDL_GetError());
		return 1;
		return false;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	return true;
}
void Core::OnEvent(SDL_Event* event) {
	if (event->type == SDL_QUIT) {
		running = false;
	}
}
void Core::OnLoop() {
	std::cout << "TEST" << std::endl;
	SDL_Rect rect;
	rect.w = 10;
	rect.h = 10;
	rect.x = 100;
	rect.y = 100;

	SDL_SetRenderDrawColor(Core::renderer, 20, 20, 20, 255);
	SDL_RenderFillRect(Core::renderer, &rect);
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