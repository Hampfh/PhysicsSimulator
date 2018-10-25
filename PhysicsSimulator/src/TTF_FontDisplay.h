#pragma once
#include <SDL2_TTF/SDL_ttf.h>
#include <iostream>

struct TextElementList {
	SDL_Texture* textTexture = nullptr;
	SDL_Rect* textRect = nullptr;
	SDL_Color* color = nullptr;
	TextElementList* next = nullptr;

};

class FontDisplay {
	
	TextElementList* first_ = nullptr;
	TextElementList* last_ = nullptr;

public:
	FontDisplay();
	~FontDisplay();
	TextElementList* CreateTextObject(SDL_Rect box, std::string* message, std::string* font_path, int font_size);
	void DisplayText() const;
	void DeleteTextObject(TextElementList* text_object);
};