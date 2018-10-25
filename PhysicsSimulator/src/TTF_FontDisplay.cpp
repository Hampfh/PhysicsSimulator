#include "TTF_FontDisplay.h"
#include "Core.h"

FontDisplay::FontDisplay() {
	TTF_Init();

}

FontDisplay::~FontDisplay() {
	TTF_Quit();
}

TextElementList* FontDisplay::CreateTextObject(const SDL_Rect box, std::string* message, std::string* font_path, const int font_size) {

	// Add textObject to list
	if (first_ == nullptr) {
		first_ = new TextElementList;
		last_ = first_;
	} else {
		last_->next = new TextElementList;
		last_ = last_->next;
	}

	TTF_Font* font = TTF_OpenFont(font_path->c_str(), font_size);
	// Test if font was successfully imported 
	if (!font) {
		std::cout << "Failed to load font" << std::endl;
		return nullptr;
	}
	SDL_Surface* surface = TTF_RenderText_Solid(font, message->c_str(), *last_->color);
	if (!surface) {
		std::cout << "Failed to create text surface" << std::endl;
		return nullptr;
	}

	last_->textTexture = SDL_CreateTextureFromSurface(Core::renderer_, surface);
	if (!last_->textTexture) {
		std::cout << "Failed to create text texture" << std::endl;
		return nullptr;
	}
	SDL_FreeSurface(surface);
	TTF_CloseFont(font);

	last_->textRect = new SDL_Rect();
	last_->textRect->x = box.x;
	last_->textRect->y = box.y;
	last_->textRect->w = box.w;
	last_->textRect->h = box.h;

	last_->color = new SDL_Color();

	if (SDL_QueryTexture(last_->textTexture, nullptr, nullptr, &last_->textRect->w, &last_->textRect->h) != 0) {
		std::cout << "QueryTexture not loading" << std::endl;
		return nullptr;
	}
	return last_;
}

void FontDisplay::DisplayText() const {
	// Copy out text to screen
	SDL_RenderCopy(Core::renderer_, last_->textTexture, nullptr, last_->textRect);
	// Delete the texture after display
	SDL_DestroyTexture(last_->textTexture);
}

void FontDisplay::DeleteTextObject(TextElementList* text_object) {
	SDL_DestroyTexture(text_object->textTexture);
	auto current = first_;
	auto prev = first_;
	while (current != nullptr) {

		if (current == text_object) {
			if (current == first_) {
				first_ = first_->next;
			} else if (current == last_){
				last_ = prev;
			} else {
				prev->next = current->next;
			}
			// Delete current
			delete current->color;
			delete current->textRect;
			delete current;
		}

		prev = current;
		current = current->next;
	}
}