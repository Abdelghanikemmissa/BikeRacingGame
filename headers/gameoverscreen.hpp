#pragma once
#include <SDL2/SDL.h>
#include "button.hpp"

class GameOverScreen {
public:
GameOverScreen(SDL_Renderer* renderer, const char* backgroundPath);
    bool init(SDL_Renderer* renderer, const char* backgroundPath);
    void render(SDL_Renderer* renderer);
    bool handleEvent(SDL_Event& e);
    void clean();

private:
    SDL_Texture* background;
    Button yesButton;
    Button noButton;
};