#pragma once
#include <SDL2\SDL.h>
#include <stdio.h>
#include <iostream>

class Core {
private:
	friend class PhysicsEngine;
	friend class PhysicsObject;
	
	const int screenWidth = 800;
	const int screenHeight = 600;

	bool running;
	bool pause;
	SDL_Window* window;
	static SDL_Renderer* renderer;

	PhysicsObject* selectedObject = nullptr;

	PhysicsEngine* pe = nullptr;
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
