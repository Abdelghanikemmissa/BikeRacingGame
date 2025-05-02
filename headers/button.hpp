#pragma once
#include <SDL2/SDL.h>

class Button {
public:
    Button(SDL_Renderer* renderer, const char* imagePath, SDL_Rect rect);
    ~Button();
    
    void render(SDL_Renderer* renderer);
    bool isClicked(int mouseX, int mouseY) const;
    
private:
    SDL_Texture* texture;
    SDL_Rect rect;
};