#include "../headers/Obstacle.hpp"
#include <SDL2/SDL_image.h>
#include <SDL2/SDL.h>
#include <cstdlib>

Obstacle::Obstacle() : texture(nullptr), speed(0), active(false) {}

void Obstacle::spawn(int screenWidth, int groundY) {
    position.x = screenWidth + (rand() % 500);
    position.y = groundY - position.h;
    speed = 150 + (rand() % 100);
    active = true;
}

void Obstacle::update(float deltaTime, float scrollSpeed) {
    if (active) {
        position.x -= (speed + scrollSpeed) * deltaTime;
        if (position.x + position.w < 0) {
            active = false;
        }
    }
}

void Obstacle::render(SDL_Renderer* renderer) {
    if (active) {
        SDL_RenderCopy(renderer, texture, nullptr, &position);
    }
}

SDL_Rect Obstacle::getCollider() const {
    return position;
}

bool Obstacle::isActive() const {
    return active;
}

void Obstacle::clean() {
    if (texture) {
        SDL_DestroyTexture(texture);
        texture = nullptr;
    }
}

void Obstacle::reset() {
    active = false;
}
void Obstacle::setSize(int w, int h) {
    position.w = w;
    position.h = h;
}

bool Obstacle::load(SDL_Renderer* renderer, const char* path) {
    texture = IMG_LoadTexture(renderer, path);
    if (!texture) {
        return false;
    }
    
    // Obtenez la taille originale de la texture
    int originalW, originalH;
    SDL_QueryTexture(texture, nullptr, nullptr, &originalW, &originalH);
    
    // Définissez la taille souhaitée (ex: 50% de la taille originale)
    position.w = originalW * 0.5;  // Réduit la largeur de moitié
    position.h = originalH * 0.5;  // Réduit la hauteur de moitié
    
    return true;
}