#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <cmath>
#include <vector>
#include <cstdlib>
#include "../headers/player.hpp"
#include "../headers/button.hpp"
#include "../headers/Obstacle.hpp"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 700;
const int GROUND_HEIGHT = SCREEN_HEIGHT - 10;
const float OBSTACLE_SPAWN_INTERVAL = 5.0f;
const float SCROLL_SPEED = 250.0f;

struct GameState {
    bool showIntro = true;
    bool inMenu = false;
    bool inGame = false;
    bool gameOver = false;
    Uint32 gameOverStartTime = 0;
};

class ScrollingBackground {
public:
    bool load(SDL_Renderer* renderer, const char* path) {
        texture = IMG_LoadTexture(renderer, path);
        if (!texture) return false;
        SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
        return true;
    }

    void render(SDL_Renderer* renderer, float offset) {
        float wrappedOffset = fmodf(offset, width);
        SDL_Rect dest1 = {static_cast<int>(wrappedOffset), 0, width, height};
        SDL_RenderCopy(renderer, texture, nullptr, &dest1);
        
        if (wrappedOffset > 0) {
            SDL_Rect dest2 = {static_cast<int>(wrappedOffset) - width, 0, width, height};
            SDL_RenderCopy(renderer, texture, nullptr, &dest2);
        } else {
            SDL_Rect dest2 = {static_cast<int>(wrappedOffset) + width, 0, width, height};
            SDL_RenderCopy(renderer, texture, nullptr, &dest2);
        }
    }

    void clean() {
        if (texture) SDL_DestroyTexture(texture);
    }

private:
    SDL_Texture* texture = nullptr;
    int width, height;
};

bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
    return (a.x < b.x + b.w &&
            a.x + a.w > b.x &&
            a.y < b.y + b.h &&
            a.y + a.h > b.y);
}

int main(int argc, char* argv[]) {
    // Initialisation SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL init error: " << SDL_GetError() << std::endl;
        return 1;
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "SDL_image init error: " << IMG_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // Création fenêtre et renderer
    SDL_Window* window = SDL_CreateWindow("Bike Racing Game",
                                        SDL_WINDOWPOS_CENTERED,
                                        SDL_WINDOWPOS_CENTERED,
                                        SCREEN_WIDTH, SCREEN_HEIGHT,
                                        SDL_WINDOW_SHOWN);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
                                              SDL_RENDERER_ACCELERATED | 
                                              SDL_RENDERER_PRESENTVSYNC);

    // Initialisation des systèmes
    ScrollingBackground roadBg;
    roadBg.load(renderer, "pictures/background.png");

    // Initialisation obstacles
    std::vector<Obstacle> obstacles;
    for (int i = 0; i < 5; ++i) {
        Obstacle obs;
        obs.load(renderer, "pictures/box4.png");
        obs.setSize(120, 120);
        obstacles.push_back(obs);
    }
    float obstacleSpawnTimer = 0;

    // Chargement autres assets
    SDL_Texture* introBg = IMG_LoadTexture(renderer, "pictures/motobg.webp");
    SDL_Texture* menuBg = IMG_LoadTexture(renderer, "pictures/BGR.jpg");
    SDL_Texture* gameover = nullptr; // Initialisation différée

    // Création boutons menu
    Button playButton(renderer, "pictures/play.png", 
                     {SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 450, 600});
    Button exitButton(renderer, "pictures/quit.png", 
                     {SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 100, 450, 600});

    // Initialisation joueur
    Player player;
    player.init(renderer, "pictures/b1.png");
    player.setSize(200, 200);
    player.setGroundPosition(GROUND_HEIGHT);

    // Boucle de jeu
    GameState gameState;
    float roadOffset = 0;
    bool running = true;
    SDL_Event e;
    Uint32 lastTime = SDL_GetTicks();

    while (running) {
        float deltaTime = (SDL_GetTicks() - lastTime) / 1000.0f;
        lastTime = SDL_GetTicks();

        // Gestion événements
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (gameState.inMenu) {
                if (e.type == SDL_MOUSEBUTTONDOWN) {
                    int x, y;
                    SDL_GetMouseState(&x, &y);

                    if (playButton.isClicked(x, y)) {
                        gameState.inMenu = false;
                        gameState.inGame = true;
                        player.reset();
                        roadOffset = 0;
                        for (auto& obs : obstacles) obs.reset();
                    } else if (exitButton.isClicked(x, y)) {
                        running = false;
                    }
                }
            }
            else if (gameState.inGame) {
                player.handleEvent(e);
                
                if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
                    gameState.inGame = false;
                    gameState.inMenu = true;
                }
            }
        }

        // Mise à jour jeu
        if (gameState.showIntro && SDL_GetTicks() > 4000) {
            gameState.showIntro = false;
            gameState.inMenu = true;
        }

        if (gameState.inGame) {
            roadOffset -= SCROLL_SPEED * deltaTime;
            player.update(deltaTime);

            // Gestion obstacles
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

            // Collisions
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

        // Rendu
        SDL_RenderClear(renderer);
        
        if (gameState.showIntro) {
            SDL_RenderCopy(renderer, introBg, nullptr, nullptr);
        }
        else if (gameState.inMenu) {
            SDL_RenderCopy(renderer, menuBg, nullptr, nullptr);
            playButton.render(renderer);
            exitButton.render(renderer);
        }
        else if (gameState.inGame) {
            roadBg.render(renderer, roadOffset);
            player.render(renderer);
            for (auto& obs : obstacles) obs.render(renderer);
        }
        else if (gameState.gameOver) {
            if (gameover) {
                SDL_RenderCopy(renderer, gameover, nullptr, nullptr);
            }
            
            // Retour au menu après 3 secondes
            if (SDL_GetTicks() - gameState.gameOverStartTime > 3000) {
                gameState.gameOver = false;
                gameState.inMenu = true;
                SDL_DestroyTexture(gameover);
                gameover = nullptr;
            }
        }
        
        SDL_RenderPresent(renderer);
        // SDL_Delay(16);
    }

    // Nettoyage
    if (gameover) SDL_DestroyTexture(gameover);
    for (auto& obs : obstacles) obs.clean();
    roadBg.clean();
    SDL_DestroyTexture(introBg);
    SDL_DestroyTexture(menuBg);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();

    return 0;
}