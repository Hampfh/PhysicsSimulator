#pragma once
#include <SDL2/SDL.h>
#include "PhysicsEngine.h"
#include "Universe.h"
#include "TTF_FontDisplay.h"
#include <iostream>
#include <locale> // std::isdigit
#include <vector>
// For console reading
#include <vector>
#include <thread>
#include <sstream>  

struct TextPackage;

class Universe;

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

	PhysicsObject* hoverObject_ = nullptr;
	PhysicsObject* selectedObject_ = nullptr;
	int selectedObjectAction_ = 0;

	static PhysicsEngine* pe_;
	Universe* universe_ = nullptr;
	FontDisplay* textDisplay_ = nullptr;
public:
	Core();
	int OnExecute();
	bool OnInit();
	void OnEvent(SDL_Event* event);
	void OnLoop();
	static void OnRender();
	void OnCleanUp() const;

	static void DrawPauseLogo(int x, int y, SDL_Color color);
	void DrawSettingPackage(TextPackage* package) const;
	void CheckConsole() const;
	void ConsoleInterpretation(std::string* command) const;
	static bool IsNumber(const std::string& s);
	void UpdateGraphics() const;
};



