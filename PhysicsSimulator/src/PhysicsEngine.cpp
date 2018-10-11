#include "PhysicsEngine.h"

PhysicsEngine::PhysicsEngine() {

}


PhysicsEngine::~PhysicsEngine() {

}

int PhysicsEngine::AddBox(int width, int height) {
	int* x = nullptr;
	int* y = nullptr;
	SDL_GetMouseState(x, y);
	
	SDL_Rect rect;
	rect.w = width;
	rect.h = height;
	rect.x = *x;
	rect.y = *y;

	SDL_SetRenderDrawColor(Core::renderer, 20, 20, 20, 255);
	SDL_RenderFillRect(Core::renderer, &rect);
	return 0;
}
