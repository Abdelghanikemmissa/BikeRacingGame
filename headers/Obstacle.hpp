#pragma once
#include <SDL2/SDL.h>

class Obstacle {
public:
    Obstacle();
    
    bool load(SDL_Renderer* renderer, const char* path);
    void spawn(int screenWidth, int groundY);
    void update(float deltaTime, float scrollSpeed);
    void render(SDL_Renderer* renderer);
    void setSize(int w, int h);
    void clean();
    SDL_Rect getCollider() const;
    bool isActive() const;  // Ajoutez cette méthode
    void reset();

private:
    SDL_Texture* texture;
    SDL_Rect position;
    int speed;
    bool active;
};