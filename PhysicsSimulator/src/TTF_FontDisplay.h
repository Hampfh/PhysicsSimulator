#pragma once
#include <SDL2_TTF/SDL_ttf.h>
#include <iostream>

class FontDisplay {
	TTF_Font* font_ = nullptr;
	SDL_Surface* surface_ = nullptr;
	SDL_Texture* texture_ = nullptr;
	SDL_Color* currentColor_ = nullptr;
	SDL_Rect* textRect_ = nullptr;

public:
	FontDisplay();
	~FontDisplay();
	void SetFontColor(SDL_Color* color);
	int CreateText(SDL_Point location, SDL_Point size, std::string* message, std::string* font_path, int font_size);
	void DisplayText() const;
};