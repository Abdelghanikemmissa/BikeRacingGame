#include "../headers/Timer.hpp"
#include <sstream>
#include <iomanip>

Timer::Timer() : initialTime(120000), // 2 minutes en ms
                 remainingTime(initialTime),
                 isRunning(false),
                 startTime(0),
                 textColor({255, 255, 255, 255}), // Blanc par défaut
                 font(nullptr) {
    position = {20, 20, 0, 0}; // Position par défaut
}

void Timer::start() {
    isRunning = true;
    startTime = SDL_GetTicks();
}

void Timer::update(float deltaTime) {
    if (isRunning) {
        Uint32 currentTime = SDL_GetTicks();
        Uint32 elapsed = currentTime - startTime;
        
        if (elapsed >= remainingTime) {
            remainingTime = 0;
            isRunning = false;
        } else {
            remainingTime = initialTime - elapsed;
        }
    }
}

void Timer::render(SDL_Renderer* renderer) {
    if (!font) return;
    
    std::string timeText = getFormattedTime();
    SDL_Surface* surface = TTF_RenderText_Solid(font, timeText.c_str(), textColor);
    if (!surface) return;
    
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }
    
    SDL_QueryTexture(texture, nullptr, nullptr, &position.w, &position.h);
    SDL_RenderCopy(renderer, texture, nullptr, &position);
    
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

bool Timer::isTimeUp() const {
    return remainingTime <= 0;
}

void Timer::reset() {
    remainingTime = initialTime;
    isRunning = false;
}

void Timer::setPosition(int x, int y) {
    position.x = x;
    position.y = y;
}

void Timer::setFont(TTF_Font* font) {
    this->font = font;
}

void Timer::setColor(SDL_Color color) {
    textColor = color;
}

std::string Timer::getFormattedTime() const {
    Uint32 seconds = remainingTime / 1000;
    Uint32 minutes = seconds / 60;
    seconds %= 60;
    
    std::ostringstream oss;
    oss << std::setw(2) << std::setfill('0') << minutes << ":"
        << std::setw(2) << std::setfill('0') << seconds;
    return oss.str();
}