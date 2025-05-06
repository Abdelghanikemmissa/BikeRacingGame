#include "../headers/score.hpp"
#include <sstream>

Score::Score()
    : scoreValue(0), timer(0), texture(nullptr), renderer(nullptr), font(nullptr) {
    color = {255, 255, 255, 255}; // White
    rect = {10, 10, 0, 0};
}

Score::~Score() {
    if (texture) SDL_DestroyTexture(texture);
    if (font) TTF_CloseFont(font);
}

bool Score::init(SDL_Renderer* ren) {
    renderer = ren;
    if (TTF_Init() == -1) {
        SDL_Log("TTF_Init error: %s", TTF_GetError());
        return false;
    }

    font = TTF_OpenFont("fonts/arial.ttf", 28);  // Assure-toi que ce fichier existe
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return false;
    }

    updateTexture();
    return true;
}

void Score::update(float deltaTime) {
    timer += deltaTime;
    if (timer >= 1.0f) { // Increase score every second
        timer = 0;
        scoreValue++;
        updateTexture();
    }
}

void Score::updateTexture() {
    if (texture) SDL_DestroyTexture(texture);

    std::stringstream ss;
    ss << "Score: " << scoreValue;
    SDL_Surface* surface = TTF_RenderText_Blended(font, ss.str().c_str(), color);
    if (!surface) return;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    rect.w = surface->w;
    rect.h = surface->h;
    SDL_FreeSurface(surface);
}

void Score::render(SDL_Renderer* renderer) {
    if (texture) SDL_RenderCopy(renderer, texture, nullptr, &rect);
}

void Score::reset() {
    scoreValue = 0;
    timer = 0;
    updateTexture();
}

int Score::getValue() const {
    return scoreValue;
}
