#ifndef SCORE_HPP
#define SCORE_HPP

#include <SDL2/SDL.h>

class Score {
public:
    Score();
    void increase(int amount);
    void reset();
    int getScore() const;
    void render(SDL_Renderer* renderer, int x, int y);

private:
    int score;
};

#endif
