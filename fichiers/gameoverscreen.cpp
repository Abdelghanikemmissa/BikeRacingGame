#include "../headers/gameoverscreen.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 700;
GameOverScreen::GameOverScreen(SDL_Renderer* renderer, const char* backgroundPath)
    : yesButton(renderer, "pictures/yes.png", {SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 100, 100, 50}),
      noButton(renderer, "pictures/no.png", {SCREEN_WIDTH/2 + 50, SCREEN_HEIGHT/2 + 100, 100, 50}) {
    background = IMG_LoadTexture(renderer, backgroundPath);
}
bool GameOverScreen::init(SDL_Renderer* renderer, const char* backgroundPath) {
    background = IMG_LoadTexture(renderer, backgroundPath);
    if (!background) return false;

    yesButton = Button(renderer, "pictures/yes.png", 
                      {SCREEN_WIDTH/2 - 150, SCREEN_HEIGHT/2 + 100, 100, 50});
    noButton = Button(renderer, "pictures/no.png", 
                     {SCREEN_WIDTH/2 + 50, SCREEN_HEIGHT/2 + 100, 100, 50});
    return true;
}

void GameOverScreen::render(SDL_Renderer* renderer) {
    SDL_RenderCopy(renderer, background, nullptr, nullptr);
    yesButton.render(renderer);
    noButton.render(renderer);
}

bool GameOverScreen::handleEvent(SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int x, y;
        SDL_GetMouseState(&x, &y);
        if (yesButton.isClicked(x, y)) return true;
        if (noButton.isClicked(x, y)) return false;
    }
    return false;
}

void GameOverScreen::clean() {
    if (background) SDL_DestroyTexture(background);
}