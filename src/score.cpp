#include "../headers/score.hpp"
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>

Score::Score() : score(0) {}

void Score::increase(int amount) {
    score += amount;
}

void Score::reset() {
    score = 0;
}

int Score::getScore() const {
    return score;
}

void Score::render(SDL_Renderer* renderer, int x, int y) {
    // Affiche un petit carré rouge (comme un symbole) pour représenter le score
    SDL_Rect rect = {x, y, 100, 30};
    SDL_SetRenderDrawColor(renderer, 200, 0, 0, 255); // rouge
    SDL_RenderFillRect(renderer, &rect);

    // Tu peux aussi afficher le score en texte plus tard avec SDL_ttf
}
