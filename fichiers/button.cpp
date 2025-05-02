#include "../headers/button.hpp"
#include <SDL2/SDL_image.h>

Button::Button(SDL_Renderer* renderer, const char* imagePath, SDL_Rect rect) 
    : rect(rect) {
    texture = IMG_LoadTexture(renderer, imagePath);
    if (!texture) {
        SDL_Log("Failed to load button texture: %s", IMG_GetError());
    }
}

Button::~Button() {
    if (texture) {
        SDL_DestroyTexture(texture);
    }
}

void Button::render(SDL_Renderer* renderer) {
    if (texture) {
        SDL_RenderCopy(renderer, texture, nullptr, &rect);
    }
}

bool Button::isClicked(int mouseX, int mouseY) const {
    return (mouseX >= rect.x && mouseX <= rect.x + rect.w &&
            mouseY >= rect.y && mouseY <= rect.y + rect.h);
}