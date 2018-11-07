#pragma once
// Used for enabling sprintf function
#define _CRT_SECURE_NO_WARNINGS


#include <SDL2/SDL.h>
#include "PhysicsEngine.h"
#include "includes/Vector.h"
#include "Universe.h"
#include "TTF_FontDisplay.h"
#include "CommandInterpretor.h"
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
	NONE			= 0,
	MOVEMENT		= 1,
	SHOW_PROPERTIES = 2,
	LOCK_OBJECT		= 4,
	DRAG_SCREEN		= 8
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
	Vector2 screenOffset_;

	Vector2 dragScreen_;

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

	SDL_Color standardColor_;

	bool running_;
	bool pause_;
	SDL_Window* window_;
	static SDL_Renderer* renderer_;

	PhysicsObject* hoverObject_ = nullptr;
	PhysicsObject* selectedObject_ = nullptr;
	unsigned int simulationStates_ = 0;

	static PhysicsEngine* pe_;
	Universe* universe_ = nullptr;
	FontDisplay* textDisplay_ = nullptr;

	TextElementList* zoomText_;
public:
	Core();
	int OnExecute();
	bool OnInit();
	void OnEvent(SDL_Event* event);
	void OnLoop();
	static void OnRender();
	void OnCleanUp() const;

	void AddState(States new_state);
	void RunStates();
	void EndState(States end_state);
	static void DrawPauseLogo(int x, int y, SDL_Color color);
	void DrawSettingPackage() const;
	void StabilizeFPS();
	void UpdateGraphics() const;

	void DrawCircle(Vector2 location, float radius, SDL_Color* color, int cross_hair) const;
};

void ConvertCoordinates(Vector2* position, int origin_x, int origin_y, float zoom, int screen_width, int screen_height, Vector2 screen_offset);
void ConvertCoordinate(int* coordinate, int origin, float zoom, int screen, float offset);
void CenterOrigin(Vector2* position, int origin_x, int origin_y, int screen_width, int screen_height);
void ReverseOrigin(Vector2* position, int origin_x, int origin_y, int screen_width, int screen_height);
void CenterCoordinate(int* coordinate, int origin, int screen);
void ReverseCoordinate(int* coordinate, int origin, int screen);
void TransposePosition(Vector2* position, int origin_x, int origin_y);
void TransposeCoordinate(int* coordinate, int origin);
void ZoomPosition(Vector2* position, float zoom);
void ZoomCoordinate(int* coordinate, float zoom);