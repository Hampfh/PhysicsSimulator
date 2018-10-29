#pragma once
#include <SDL2_TTF/SDL_ttf.h>
#include <iostream>

struct TextElementList {
	SDL_Texture* textTexture = nullptr;
	SDL_Rect textRect;
	SDL_Rect mainRect;
	TextElementList* next = nullptr;
	TextElementList* prev = nullptr;
};

class FontDisplay {
	
	TextElementList* first_ = nullptr;
	TextElementList* last_ = nullptr;

public:
	FontDisplay();
	~FontDisplay();
	TextElementList* AddToQueue();
	TextElementList* CreateTextObject(SDL_Rect box, std::string* message, std::string* font_path, int font_size, SDL_Color fg);
	void DisplayText() const;
	void DisplayText(TextElementList* text_object) const;
	static void DisplayText(TextElementList* first, TextElementList* last, SDL_Rect* main_container);
	void DeleteAll() const;
	void DeleteTextObject(TextElementList* text_object);
	void DeleteTextObjects(TextElementList* first, TextElementList* last) const;
};