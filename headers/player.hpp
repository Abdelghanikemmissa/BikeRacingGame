#pragma once
#include <SDL2/SDL.h>

class Player {
public:
    Player();
    
    bool init(SDL_Renderer* renderer, const char* imagePath);
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    void handleEvent(SDL_Event& e);
    void setGroundPosition(int groundY);
    void reset();
    void setSize(int w, int h);
    SDL_Rect getRect() const;
    bool isJumping() const;
    SDL_Rect getCollider() const; 

    // Méthode pour obtenir la position X
    float getPositionX() const {
        return positionX;
    }
    
private:
    SDL_Texture* texture;
    float x, y;
    float velocityX, velocityY;
    int width, height;
    int groundLevel;
    bool jumping;
    
    const float GRAVITY = 900.0f;
    const float JUMP_FORCE = 670.0f;
    const float MOVE_SPEED = 400.0f;

    float positionX; // Position X du joueur
    float positionY; // Position Y du joueur
};