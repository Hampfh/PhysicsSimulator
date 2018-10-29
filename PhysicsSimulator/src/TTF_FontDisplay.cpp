#include "TTF_FontDisplay.h"
#include "Core.h"

FontDisplay::FontDisplay() {
	TTF_Init();

}

FontDisplay::~FontDisplay() {
	DeleteAll();
	TTF_Quit();
}

TextElementList* FontDisplay::AddToQueue() {
	// Add textObject to list
	if (first_ == nullptr) {
		first_ = new TextElementList;
		last_ = first_;
	} else {
		last_->next = new TextElementList;
		last_->next->prev = last_; // Connect prev 
		last_ = last_->next;
	}
	return last_;
}

TextElementList* FontDisplay::CreateTextObject(const SDL_Rect box, std::string* message, std::string* font_path, const int font_size, SDL_Color fg) {

	AddToQueue();

	TTF_Font* font = TTF_OpenFont(font_path->c_str(), font_size);
	// Test if font was successfully imported 
	if (!font) {
		std::cout << "Failed to load font" << std::endl;
		return nullptr;
	}
	SDL_Surface* surface = TTF_RenderText_Solid(font, message->c_str(), fg);
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

	last_->textRect.x = box.x;
	last_->textRect.y = box.y;
	std::cout << "X: " << last_->textRect.x << " Y: " << last_->textRect.y << std::endl;
	last_->textRect.w = box.w;
	last_->textRect.h = box.h;

	if (SDL_QueryTexture(last_->textTexture, nullptr, nullptr, &last_->textRect.w, &last_->textRect.h) != 0) {
		std::cout << "QueryTexture not loading" << std::endl;
		return nullptr;
	}
	return last_;
}

// Display all text element in storage
void FontDisplay::DisplayText() const {
	// Render all objects
	auto current = first_;
	while (current != nullptr) {
		SDL_RenderCopy(Core::renderer_, current->textTexture, nullptr, &current->textRect);
		std::cout << "DISPLAYED OBJECT" << std::endl;
		current = current->next;
	}
}

// Display the selected text object
void FontDisplay::DisplayText(TextElementList* text_object) const {
	// Copy out text to screen
	SDL_RenderCopy(Core::renderer_, last_->textTexture, nullptr, &last_->textRect);
}

// Display all text object from first to last
void FontDisplay::DisplayText(TextElementList* first, TextElementList* last, SDL_Rect* main_container) {
	// Render all objects
	auto current = first;

	// Calculate difference in movement
	const int deltaX = main_container->x - first->mainRect.x;
	const int deltaY = main_container->y - first->mainRect.y;

	// Update old box coordinates
	first->mainRect = *main_container;

	while (current != nullptr) {
		// Update text properties
		current->textRect.x += deltaX;
		current->textRect.y += deltaY;

		SDL_RenderCopy(Core::renderer_, current->textTexture, nullptr, &current->textRect);
		if (current == last) { break; }
		current = current->next;
	}
}

void FontDisplay::DeleteAll() const {
	auto current = first_;
	auto prev = first_->prev;

	while (current != nullptr) {
		delete prev;
		prev = current;
		current = current->next;
	}
}

void FontDisplay::DeleteTextObject(TextElementList* text_object) {
	// Delete the texture after display
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
			delete current;
		}

		prev = current;
		current = current->next;
	}
}

// Deletes all objects between first and last
void FontDisplay::DeleteTextObjects(TextElementList* first, TextElementList* last) const {
	auto current = first;
	auto prev = first;
	TextElementList* link = current->prev;

	while (current == nullptr) {
		
		delete prev;

		if (current == last) {
			// There are nodes before and after
			if (link != nullptr && last != last_) {
				link->next = last->next;
				last->next->prev = link;
			} // Only nodes before
			else if (link != nullptr && last == last_) {
				link->next = nullptr;
			} // Only nodes after 
			else if (link == nullptr && last != last_) {
				last->next = first_;
			}
			delete current;
			break;
		}

		prev = current;
		current = current->next;
	}
}
