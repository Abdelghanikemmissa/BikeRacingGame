#ifndef SCORE_HPP
#define SCORE_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

class Score {
public:
    Score();
    ~Score();
    bool init(SDL_Renderer* renderer);
    void update(float deltaTime);
    void render(SDL_Renderer* renderer);
    void reset();
    int getValue() const;

private:
    int scoreValue;
    float timer;
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    TTF_Font* font;
    SDL_Color color;
    SDL_Rect rect;

    void updateTexture();
};

#endif
