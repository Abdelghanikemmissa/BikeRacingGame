#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "../headers/player.hpp"
#include "../headers/button.hpp"
#include "../headers/Obstacle.hpp"
#include "../headers/score.hpp"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 700;
const int GROUND_HEIGHT = SCREEN_HEIGHT - 10;
const float OBSTACLE_SPAWN_INTERVAL = 2.0f;
const float SCROLL_SPEED = 300.0f;
const int FINISH_LINE_WORLD_X = 5000;

struct GameState {
    bool showIntro = true;
    bool inMenu = false;
    bool inGame = false;
    bool gameOver = false;
    bool gameWon = false;
    Uint32 gameOverStartTime = 0;
};

class ScrollingBackground {
public:
    bool load(SDL_Renderer* renderer, const char* path) {
        texture = IMG_LoadTexture(renderer, path);
        if (!texture) {
            std::cerr << "Failed to load background: " << IMG_GetError() << std::endl;
            return false;
        }
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
        return true;
    }

    void render(SDL_Renderer* renderer, float offset) {
        float wrappedOffset = fmod(offset, width);
        SDL_Rect dest1 = {static_cast<int>(wrappedOffset), 0, width, height};
        SDL_RenderCopy(renderer, texture, nullptr, &dest1);

        SDL_Rect dest2;
        if (wrappedOffset > 0) {
            dest2 = {static_cast<int>(wrappedOffset) - width, 0, width, height};
        } else {
            dest2 = {static_cast<int>(wrappedOffset) + width, 0, width, height};
        }
        SDL_RenderCopy(renderer, texture, nullptr, &dest2);
    }

    void clean() {
        if (texture) SDL_DestroyTexture(texture);
    }

private:
    SDL_Texture* texture = nullptr;
    int width = 0, height = 0;
};

bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image init error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    if (TTF_Init() == -1) {
        std::cerr << "TTF init error: " << TTF_GetError() << std::endl;
        IMG_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Bike Racing Game",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          SCREEN_WIDTH, SCREEN_HEIGHT,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cerr << "Window creation error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1,
                                                SDL_RENDERER_ACCELERATED |
                                                SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer creation error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    ScrollingBackground roadBg;
    if (!roadBg.load(renderer, "pictures/background.png")) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_Texture* introBg = IMG_LoadTexture(renderer, "pictures/motobg.webp");
    SDL_Texture* menuBg = IMG_LoadTexture(renderer, "pictures/BGR.jpg");
    SDL_Texture* gameover = nullptr;
    SDL_Texture* victoryTexture = IMG_LoadTexture(renderer, "pictures/win_screen.png");
    SDL_Texture* finishLineTexture = IMG_LoadTexture(renderer, "pictures/finish.png");

    SDL_Rect finishLineRect = {0, GROUND_HEIGHT - 300, 100, 300};

    Button playButton(renderer, "pictures/play.png",
                      {SCREEN_WIDTH/2 - 225, SCREEN_HEIGHT/2 - 205, 450, 600});
    Button exitButton(renderer, "pictures/quit.png",
                      {SCREEN_WIDTH/2 - 225, SCREEN_HEIGHT/2 + 100, 450, 600});

    Player player;
    if (!player.init(renderer, "pictures/b1.png")) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    player.setSize(200, 200);
    player.setGroundPosition(GROUND_HEIGHT);

    std::vector<Obstacle> obstacles;
    for (int i = 0; i < 40; ++i) {
        Obstacle obs;
        if (!obs.load(renderer, "pictures/box4.png")) {
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
        obs.setSize(120, 120);
        obstacles.push_back(obs);
    }

    Score score;
    if (!score.init(renderer)) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    GameState gameState;
    float roadOffset = 0;
    float obstacleSpawnTimer = 0;
    bool running = true;
    SDL_Event e;
    Uint32 lastTime = SDL_GetTicks();

    while (running) {
        float deltaTime = (SDL_GetTicks() - lastTime) / 1000.0f;
        lastTime = SDL_GetTicks();

        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (gameState.inMenu && e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                if (playButton.isClicked(x, y)) {
                    gameState.inMenu = false;
                    gameState.inGame = true;
                    player.reset();
                    roadOffset = 0;
                    for (auto& obs : obstacles) obs.reset();
                    score.reset();
                } else if (exitButton.isClicked(x, y)) {
                    running = false;
                }
            } else if (gameState.inGame) {
                player.handleEvent(e);
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                    gameState.inGame = false;
                    gameState.inMenu = true;
                }
            }
        }

        if (gameState.showIntro && SDL_GetTicks() > 4000) {
            gameState.showIntro = false;
            gameState.inMenu = true;
        }

        if (gameState.inGame) {
            roadOffset -= SCROLL_SPEED * deltaTime;
            player.update(deltaTime);
            score.update(deltaTime);

            finishLineRect.x = FINISH_LINE_WORLD_X + static_cast<int>(roadOffset);

            if (!gameState.gameWon && checkCollision(player.getCollider(), finishLineRect)) {
                gameState.inGame = false;
                gameState.gameWon = true;
                gameState.gameOverStartTime = SDL_GetTicks();
            }

            obstacleSpawnTimer += deltaTime;
            if (obstacleSpawnTimer >= OBSTACLE_SPAWN_INTERVAL) {
                obstacleSpawnTimer = 0;
                for (auto& obs : obstacles) {
                    if (!obs.isActive()) {
                        obs.spawn(SCREEN_WIDTH, GROUND_HEIGHT);
                        break;
                    }
                }
            }

            for (auto& obs : obstacles) {
                obs.update(deltaTime, SCROLL_SPEED);
                if (obs.isActive() && checkCollision(player.getCollider(), obs.getCollider())) {
                    gameState.inGame = false;
                    gameState.gameOver = true;
                    gameState.gameOverStartTime = SDL_GetTicks();
                    if (!gameover) {
                        gameover = IMG_LoadTexture(renderer, "pictures/GAME OVER.png");
                    }
                }
            }
        }

        SDL_RenderClear(renderer);

        if (gameState.showIntro && introBg) {
            SDL_RenderCopy(renderer, introBg, nullptr, nullptr);
        } else if (gameState.inMenu) {
            if (menuBg) SDL_RenderCopy(renderer, menuBg, nullptr, nullptr);
            playButton.render(renderer);
            exitButton.render(renderer);
        } else if (gameState.inGame) {
            roadBg.render(renderer, roadOffset);

            if (finishLineTexture && finishLineRect.x > -100 && finishLineRect.x < SCREEN_WIDTH) {
                SDL_RenderCopy(renderer, finishLineTexture, nullptr, &finishLineRect);
            }

            player.render(renderer);
            for (auto& obs : obstacles) obs.render(renderer);
            score.render(renderer);
        } else if (gameState.gameOver) {
            if (gameover) SDL_RenderCopy(renderer, gameover, nullptr, nullptr);
            if (SDL_GetTicks() - gameState.gameOverStartTime > 3000) {
                gameState.gameOver = false;
                gameState.inMenu = true;
                SDL_DestroyTexture(gameover);
                gameover = nullptr;
            }
        } else if (gameState.gameWon) {
            if (victoryTexture) SDL_RenderCopy(renderer, victoryTexture, nullptr, nullptr);
            else {
                SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
                SDL_RenderClear(renderer);
            }
            if (SDL_GetTicks() - gameState.gameOverStartTime > 3000) {
                gameState.gameWon = false;
                gameState.inMenu = true;
            }
        }

        SDL_RenderPresent(renderer);
    }

    if (gameover) SDL_DestroyTexture(gameover);
    if (victoryTexture) SDL_DestroyTexture(victoryTexture);
    if (finishLineTexture) SDL_DestroyTexture(finishLineTexture);
    for (auto& obs : obstacles) obs.clean();
    roadBg.clean();
    if (introBg) SDL_DestroyTexture(introBg);
    if (menuBg) SDL_DestroyTexture(menuBg);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();

    return 0;
}
