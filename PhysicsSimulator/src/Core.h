#pragma once
#include <SDL2/SDL.h>
#include "PhysicsEngine.h"
#include "includes/Vector.h"
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
	friend class Universe;

	// Window dimensions
	const int screenWidth_ = 800;
	const int screenHeight_ = 600;

	// Mouse x and y position
	int mouseX_, mouseY_;

	// Middle of zoom
	int originX_, originY_;

	// Interval in seconds, time between frames
	float timeInterval_ = 20000000;
	float metersPerPixel_ = 1.0f;

	// Requested framerate
	int fps_;
	int updateFreq_;

	// FPS
	unsigned int lastUpdated_ = 0, currentTime_ = 0;

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

	void CheckConsole(Universe* universe) const;
	void ConsoleInterpretation(std::string* command, Universe* universe) const;
	static void DrawPauseLogo(int x, int y, SDL_Color color);
	void DrawSettingPackage(TextPackage* package) const;
	static bool IsNumber(const std::string& s);
	void StabilizeFPS();
	void UpdateGraphics() const;

	void DrawCircle(Vector2 location, int radius, SDL_Color* color) const;
};

Vector2 AdjustZoomOrigin(Vector2 position, int origin_x, int origin_y, float meters_per_second);
Vector2 TransposePosition(Vector2 position, int origin_x, int origin_y);
Vector2 ZoomPosition(Vector2 position, float meters_per_pixel);
void RenderLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, float meters_per_pixel, int origin_x, int origin_y);

