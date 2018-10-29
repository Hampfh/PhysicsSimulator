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
#include <thread>
#include <sstream>  

struct TextPackage;
class Universe;

// State definition
enum States {
	DEFAULT,
	MOVEMENT,
	SHOW_PROPERTIES
};

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

	float zoom_ = 1.0f;

	bool renderCrossHair_;

	// Requested frame rate
	int fps_;
	float optimalTime_; // Optimal time between frames
	float simulationSpeed_ = 1000000.0f; // 1 is real time, higher values makes simulation go faster than reality

	// FPS
	unsigned int lastUpdated_ = 0;

	TextElementList* tempSettingStorageFirst_ = nullptr;
	TextElementList* tempSettingStorageLast_ = nullptr;

	bool running_;
	bool pause_;
	SDL_Window* window_;
	static SDL_Renderer* renderer_;

	PhysicsObject* hoverObject_ = nullptr;
	PhysicsObject* selectedObject_ = nullptr;
	States simulatorState_ = DEFAULT;

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

	void ChangeState(States new_state);
	void CheckConsole(Universe* universe) const;
	void ConsoleInterpretation(std::string* command, Universe* universe) const;
	static void DrawPauseLogo(int x, int y, SDL_Color color);
	void DrawSettingPackage() const;
	void EndState();
	static bool IsNumber(const std::string& s);
	void StabilizeFPS();
	void UpdateGraphics() const;

	void DrawCircle(Vector2 location, float radius, SDL_Color* color, int cross_hair) const;
};

void ConvertCoordinates(Vector2* position, int origin_x, int origin_y, float zoom);
void ConvertCoordinate(int* coordinate, int origin, float zoom);
void TransposePosition(Vector2* position, int origin_x, int origin_y);
void TransposeCoordinate(int* coordinate, int origin);
void ZoomPosition(Vector2* position, float zoom);
void ZoomCoordinate(int* coordinate, float zoom);
void RenderLine(SDL_Renderer* renderer, int x1, int y1, int x2, int y2, float zoom, int origin_x, int origin_y);

