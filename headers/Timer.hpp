#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Timer {
public:
    Timer();
    void start();
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    bool isTimeUp() const;
    void reset();
    void setPosition(int x, int y);
    void setFont(TTF_Font* font);
    void setColor(SDL_Color color);
    bool isRunning() const { return isRunning; }

private:
    Uint32 initialTime; // Temps initial en millisecondes (2 minutes)
    Uint32 remainingTime;
    bool isRunning;
    Uint32 startTime;
    
    SDL_Color textColor;
    TTF_Font* font;
    SDL_Rect position;
    bool isRunning;
    
    std::string getFormattedTime() const;
};