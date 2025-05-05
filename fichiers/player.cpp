#include "../headers/player.hpp"
#include <iostream>
#include <SDL2/SDL_image.h>
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 600;
Player::Player() : 
    texture(nullptr), x(100), y(0), 
    velocityX(0), velocityY(0), 
    width(60), height(90), jumping(false) {}

bool Player::init(SDL_Renderer* renderer, const char* imagePath) {
    texture = IMG_LoadTexture(renderer, imagePath);
    if (!texture) return false;
    
    SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
    return true;
}

void Player::update(float deltaTime) {
    // Mouvement horizontal
    x += velocityX * deltaTime;
    positionX += velocityX * deltaTime;
    if (positionX < 0) positionX = 0;
    if (positionX > SCREEN_WIDTH) positionX = SCREEN_WIDTH;
    // Appliquer la gravité
    if (y < groundLevel || velocityY < 0) {
        velocityY += GRAVITY * deltaTime;
        y += velocityY * deltaTime;
    }
    
    // Collision avec le sol
    if (y >= groundLevel) {
        y = groundLevel;
        velocityY = 0;
        jumping = false;
    }
    
    // Limites écran
    if (x < 0) x = 0;
    if (x > SCREEN_WIDTH - width) x = SCREEN_WIDTH - width;
}

void Player::render(SDL_Renderer* renderer) {
    SDL_Rect dest = {static_cast<int>(x), static_cast<int>(y), width, height};
    SDL_RenderCopy(renderer, texture, nullptr, &dest);
}





SDL_Rect Player::getCollider() const {
    return {
        static_cast<int>(x),
        static_cast<int>(y), 
        width,
        height
    };
}





void Player::handleEvent(SDL_Event& e) {
    if (e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
        bool isDown = (e.type == SDL_KEYDOWN);
        
        switch (e.key.keysym.sym) {
            case SDLK_LEFT: 
                velocityX = isDown ? -MOVE_SPEED : 0; 
                break;
            case SDLK_RIGHT: 
                velocityX = isDown ? MOVE_SPEED : 0; 
                break;
            case SDLK_SPACE:
                if (isDown && !jumping) {
                    velocityY = -JUMP_FORCE;
                    jumping = true;
                }
                break;
        }
    }
}
void Player::setSize(int w, int h) {
    width = w;
    height = h;
}
void Player::setGroundPosition(int groundY) {
    groundLevel = groundY - height;
    if (y > groundLevel || !jumping) {
        y = groundLevel;
    }
}

void Player::reset() {
    x = 100;
    y = groundLevel;
    velocityX = 0;
    velocityY = 0;
    jumping = false;
}

SDL_Rect Player::getRect() const {
    return {static_cast<int>(x), static_cast<int>(y), width, height};
}

bool Player::isJumping() const {
    return jumping;
}