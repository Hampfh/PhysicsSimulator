#pragma once
#include <SDL2\SDL.h>
#include <stdio.h>
#include <iostream>

class Core {
private:
	bool running;
	SDL_Window* window;
	SDL_Renderer* renderer;
public:
	Core();
	int OnExecute();
public:
	bool OnInit();
	void OnEvent(SDL_Event* event);
	void OnLoop();
	void OnRender();
	void OnCleanUp();
};
