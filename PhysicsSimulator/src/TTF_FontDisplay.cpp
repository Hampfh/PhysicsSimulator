#include "TTF_FontDisplay.h"
#include "Core.h"

FontDisplay::FontDisplay() {
	TTF_Init();

	currentColor_ = new SDL_Color();
	textRect_ = new SDL_Rect;
}

FontDisplay::~FontDisplay() {
	TTF_Quit();
}


void FontDisplay::SetFontColor(SDL_Color* color) {
	this->currentColor_ = color;
}

int FontDisplay::CreateText(const SDL_Point location, const SDL_Point size, std::string* message, std::string* font_path, const int font_size) {
	font_ = TTF_OpenFont(font_path->c_str(), font_size);
	// Test if font was successfully imported 
	if (!font_) {
		std::cout << "Failed to load font" << std::endl;
		return 1;
	}
	surface_ = TTF_RenderText_Solid(font_, message->c_str(), *currentColor_);
	if (!surface_) {
		std::cout << "Failed to create text surface" << std::endl;
		return 1;
	}

	// Clear texture if not empty
	if (texture_ != nullptr) {
		SDL_DestroyTexture(texture_);
	}

	texture_ = SDL_CreateTextureFromSurface(Core::renderer_, surface_);
	if (!texture_) {
		std::cout << "Failed to create text texture" << std::endl;
		return 1;
	}
	SDL_FreeSurface(surface_);
	TTF_CloseFont(font_);

	textRect_->x = location.x;
	textRect_->y = location.y;
	textRect_->w = size.x;
	textRect_->h = size.y;

	if (SDL_QueryTexture(texture_, nullptr, nullptr, &textRect_->w, &textRect_->h) != 0) {
		std::cout << "QueryTexture not loading" << std::endl;
		return 1;
	}
	return 0;
}

void FontDisplay::DisplayText() const {
	SDL_RenderCopy(Core::renderer_, texture_, nullptr, textRect_);
	// Delete the texture after display
	SDL_DestroyTexture(texture_);
}