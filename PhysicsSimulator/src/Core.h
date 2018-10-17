#pragma once
#include <SDL2/SDL.h>
#include <iostream>

class Core {
	friend class PhysicsEngine;
	friend class PhysicsObject;
	friend class FontDisplay;

	const int screenWidth_ = 800;
	const int screenHeight_ = 600;

	int mouseX_, mouseY_;

	bool running_;
	bool pause_;
	SDL_Window* window_;
	static SDL_Renderer* renderer_;

	PhysicsObject* selectedObject_ = nullptr;
	int selectedObjectAction_ = 0;

	PhysicsEngine* pe_ = nullptr;
	FontDisplay* textDisplay_ = nullptr;
public:
	Core();
	int OnExecute();
	bool OnInit();
	void OnEvent(SDL_Event* event);
	void OnLoop();
	void OnRender() const;
	void OnCleanUp() const;
};



