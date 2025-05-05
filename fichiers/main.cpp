// #include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
// #include <SDL2/SDL_ttf.h>
// #include <iostream>
// #include <cmath>
// #include <vector>
// #include <cstdlib>
// #include "../headers/player.hpp"
// #include "../headers/button.hpp"
// #include "../headers/Obstacle.hpp"

// const int SCREEN_WIDTH = 1000;
// const int SCREEN_HEIGHT = 700;
// const int GROUND_HEIGHT = SCREEN_HEIGHT - 10;
// const float OBSTACLE_SPAWN_INTERVAL = 5.0f;
// const float SCROLL_SPEED = 250.0f;

// struct GameState {
//     bool showIntro = true;
//     bool inMenu = false;
//     bool inGame = false;
//     bool gameOver = false;
//     Uint32 gameOverStartTime = 0;
// };

// class ScrollingBackground {
// public:
//     bool load(SDL_Renderer* renderer, const char* path) {
//         texture = IMG_LoadTexture(renderer, path);
//         if (!texture) return false;
//         SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
//         return true;
//     }

//     void render(SDL_Renderer* renderer, float offset) {
//         float wrappedOffset = fmodf(offset, width);
//         SDL_Rect dest1 = {static_cast<int>(wrappedOffset), 0, width, height};
//         SDL_RenderCopy(renderer, texture, nullptr, &dest1);
        
//         if (wrappedOffset > 0) {
//             SDL_Rect dest2 = {static_cast<int>(wrappedOffset) - width, 0, width, height};
//             SDL_RenderCopy(renderer, texture, nullptr, &dest2);
//         } else {
//             SDL_Rect dest2 = {static_cast<int>(wrappedOffset) + width, 0, width, height};
//             SDL_RenderCopy(renderer, texture, nullptr, &dest2);
//         }
//     }

//     void clean() {
//         if (texture) SDL_DestroyTexture(texture);
//     }

// private:
//     SDL_Texture* texture = nullptr;
//     int width, height;
// };

// bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
//     return (a.x < b.x + b.w &&
//             a.x + a.w > b.x &&
//             a.y < b.y + b.h &&
//             a.y + a.h > b.y);
// }

// int main(int argc, char* argv[]) {
//     // Initialisation SDL
//     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//         std::cerr << "SDL init error: " << SDL_GetError() << std::endl;
//         return 1;
//     }

//     if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
//         std::cerr << "SDL_image init error: " << IMG_GetError() << std::endl;
//         SDL_Quit();
//         return 1;
//     }

//     // Création fenêtre et renderer
//     SDL_Window* window = SDL_CreateWindow("Bike Racing Game",
//                                         SDL_WINDOWPOS_CENTERED,
//                                         SDL_WINDOWPOS_CENTERED,
//                                         SCREEN_WIDTH, SCREEN_HEIGHT,
//                                         SDL_WINDOW_SHOWN);
//     if (!window) {
//         std::cerr << "Window creation error: " << SDL_GetError() << std::endl;
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
//                                               SDL_RENDERER_ACCELERATED | 
//                                               SDL_RENDERER_PRESENTVSYNC);
//     if (!renderer) {
//         std::cerr << "Renderer creation error: " << SDL_GetError() << std::endl;
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }

//     // Initialisation des systèmes
//     ScrollingBackground roadBg;
//     if (!roadBg.load(renderer, "pictures/background.png")) {
//         std::cerr << "Failed to load background!" << std::endl;
//         SDL_DestroyRenderer(renderer);
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }

//     // Initialisation obstacles
//     std::vector<Obstacle> obstacles;
//     for (int i = 0; i < 5; ++i) {
//         Obstacle obs;
//         if (!obs.load(renderer, "pictures/box4.png")) {
//             std::cerr << "Failed to load obstacle texture!" << std::endl;
//             SDL_DestroyRenderer(renderer);
//             SDL_DestroyWindow(window);
//             SDL_Quit();
//             return 1;
//         }
//         obs.setSize(120, 120);
//         obstacles.push_back(obs);
//     }
//     float obstacleSpawnTimer = 0;

//     // Chargement autres assets
//     SDL_Texture* introBg = IMG_LoadTexture(renderer, "pictures/motobg.webp");
//     if (!introBg) {
//         std::cerr << "Failed to load intro background!" << std::endl;
//     }

//     SDL_Texture* menuBg = IMG_LoadTexture(renderer, "pictures/BGR.jpg");
//     if (!menuBg) {
//         std::cerr << "Failed to load menu background!" << std::endl;
//     }

//     SDL_Texture* gameover = nullptr;

//     // Création boutons menu
//     Button playButton(renderer, "pictures/play.png", 
//                      {SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 450, 600});
//     Button exitButton(renderer, "pictures/quit.png", 
//                      {SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 100, 450, 600});
//     // Initialisation joueur
//     Player player;
//     if (!player.init(renderer, "pictures/b1.png")) {
//         std::cerr << "Failed to load player texture!" << std::endl;
//         SDL_DestroyRenderer(renderer);
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }
//     player.setSize(200, 200);
//     player.setGroundPosition(GROUND_HEIGHT);

//     // Boucle de jeu
//     GameState gameState;
//     float roadOffset = 0;
//     bool running = true;
//     SDL_Event e;
//     Uint32 lastTime = SDL_GetTicks();

//     while (running) {
//         float deltaTime = (SDL_GetTicks() - lastTime) / 1000.0f;
//         lastTime = SDL_GetTicks();

//         // Gestion événements
//         while (SDL_PollEvent(&e)) {
//             if (e.type == SDL_QUIT) {
//                 running = false;
//             }

//             if (gameState.inMenu) {
//                 if (e.type == SDL_MOUSEBUTTONDOWN) {
//                     int x, y;
//                     SDL_GetMouseState(&x, &y);

//                     if (playButton.isClicked(x, y)) {
//                         gameState.inMenu = false;
//                         gameState.inGame = true;
//                         player.reset();
//                         roadOffset = 0;
//                         for (auto& obs : obstacles) obs.reset();
//                     } else if (exitButton.isClicked(x, y)) {
//                         running = false;
//                     }
//                 }
//             }
//             else if (gameState.inGame) {
//                 player.handleEvent(e);
                
//                 if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
//                     gameState.inGame = false;
//                     gameState.inMenu = true;
//                 }
//             }
//         }

//         // Mise à jour jeu
//         if (gameState.showIntro && SDL_GetTicks() > 4000) {
//             gameState.showIntro = false;
//             gameState.inMenu = true;
//         }

//         if (gameState.inGame) {
//             roadOffset -= SCROLL_SPEED * deltaTime;
//             player.update(deltaTime);

//             // Gestion obstacles
//             obstacleSpawnTimer += deltaTime;
//             if (obstacleSpawnTimer >= OBSTACLE_SPAWN_INTERVAL) {
//                 obstacleSpawnTimer = 0;
//                 for (auto& obs : obstacles) {
//                     if (!obs.isActive()) {
//                         obs.spawn(SCREEN_WIDTH, GROUND_HEIGHT);
//                         break;
//                     }
//                 }
//             }

//             // Collisions
//             for (auto& obs : obstacles) {
//                 obs.update(deltaTime, SCROLL_SPEED);
                
//                 if (obs.isActive() && checkCollision(player.getCollider(), obs.getCollider())) {
//                     gameState.inGame = false;
//                     gameState.gameOver = true;
//                     gameState.gameOverStartTime = SDL_GetTicks();
//                     if (!gameover) {
//                         gameover = IMG_LoadTexture(renderer, "pictures/GAME OVER.png");
//                     }
//                 }
//             }
//         }

//         // Rendu
//         SDL_RenderClear(renderer);
        
//         if (gameState.showIntro) {
//             if (introBg) SDL_RenderCopy(renderer, introBg, nullptr, nullptr);
//         }
//         else if (gameState.inMenu) {
//             if (menuBg) SDL_RenderCopy(renderer, menuBg, nullptr, nullptr);
//             playButton.render(renderer);
//             exitButton.render(renderer);
//         }
//         else if (gameState.inGame) {
//             roadBg.render(renderer, roadOffset);
//             player.render(renderer);
//             for (auto& obs : obstacles) obs.render(renderer);
//         }
//         else if (gameState.gameOver) {
//             if (gameover) {
//                 SDL_RenderCopy(renderer, gameover, nullptr, nullptr);
//             }
            
//             // Retour au menu après 3 secondes
//             if (SDL_GetTicks() - gameState.gameOverStartTime > 5000) {
//                 gameState.gameOver = false;
//                 gameState.inMenu = true;
//                 SDL_DestroyTexture(gameover);
//                 gameover = nullptr;
//             }
//         }
        
//         SDL_RenderPresent(renderer);
//     }

//     // Nettoyage
//     if (gameover) SDL_DestroyTexture(gameover);
//     for (auto& obs : obstacles) obs.clean();
//     roadBg.clean();
//     if (introBg) SDL_DestroyTexture(introBg);
//     if (menuBg) SDL_DestroyTexture(menuBg);
//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     IMG_Quit();
//     SDL_Quit();

//     return 0;
// }





// #include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
// #include <SDL2/SDL_ttf.h>
// #include <iostream>
// #include <cmath>
// #include <vector>
// #include <cstdlib>
// #include "../headers/player.hpp"
// #include "../headers/button.hpp"
// #include "../headers/Obstacle.hpp"

// const int SCREEN_WIDTH = 1000;
// const int SCREEN_HEIGHT = 700;
// const int GROUND_HEIGHT = SCREEN_HEIGHT - 10;
// const float OBSTACLE_SPAWN_INTERVAL = 5.0f;
// const float SCROLL_SPEED = 250.0f;

// struct GameState {
//     bool showIntro = true;
//     bool inMenu = false;
//     bool inGame = false;
//     bool gameOver = false;
//     Uint32 gameOverStartTime = 0;
// };

// class ScrollingBackground {
// public:
//     bool load(SDL_Renderer* renderer, const char* path) {
//         texture = IMG_LoadTexture(renderer, path);
//         if (!texture) return false;
//         SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
//         return true;
//     }

//     void render(SDL_Renderer* renderer, float offset) {
//         float wrappedOffset = fmodf(offset, width);
//         SDL_Rect dest1 = {static_cast<int>(wrappedOffset), 0, width, height};
//         SDL_RenderCopy(renderer, texture, nullptr, &dest1);
        
//         if (wrappedOffset > 0) {
//             SDL_Rect dest2 = {static_cast<int>(wrappedOffset) - width, 0, width, height};
//             SDL_RenderCopy(renderer, texture, nullptr, &dest2);
//         } else {
//             SDL_Rect dest2 = {static_cast<int>(wrappedOffset) + width, 0, width, height};
//             SDL_RenderCopy(renderer, texture, nullptr, &dest2);
//         }
//     }

//     void clean() {
//         if (texture) SDL_DestroyTexture(texture);
//     }

// private:
//     SDL_Texture* texture = nullptr;
//     int width, height;
// };

// bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
//     return (a.x < b.x + b.w &&
//             a.x + a.w > b.x &&
//             a.y < b.y + b.h &&
//             a.y + a.h > b.y);
// }

// int main(int argc, char* argv[]) {
//     // Initialisation SDL
//     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//         std::cerr << "SDL init error: " << SDL_GetError() << std::endl;
//         return 1;
//     }

//     if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
//         std::cerr << "SDL_image init error: " << IMG_GetError() << std::endl;
//         SDL_Quit();
//         return 1;
//     }

//     // Création fenêtre et renderer
//     SDL_Window* window = SDL_CreateWindow("Bike Racing Game",
//                                         SDL_WINDOWPOS_CENTERED,
//                                         SDL_WINDOWPOS_CENTERED,
//                                         SCREEN_WIDTH, SCREEN_HEIGHT,
//                                         SDL_WINDOW_SHOWN);
//     if (!window) {
//         std::cerr << "Window creation error: " << SDL_GetError() << std::endl;
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
//                                               SDL_RENDERER_ACCELERATED | 
//                                               SDL_RENDERER_PRESENTVSYNC);
//     if (!renderer) {
//         std::cerr << "Renderer creation error: " << SDL_GetError() << std::endl;
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }

//     // Initialisation des systèmes
//     ScrollingBackground roadBg;
//     if (!roadBg.load(renderer, "pictures/background.png")) {
//         std::cerr << "Failed to load background!" << std::endl;
//         SDL_DestroyRenderer(renderer);
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }

//     // Initialisation obstacles
//     std::vector<Obstacle> obstacles;
//     for (int i = 0; i < 5; ++i) {
//         Obstacle obs;
//         if (!obs.load(renderer, "pictures/box4.png")) {
//             std::cerr << "Failed to load obstacle texture!" << std::endl;
//             SDL_DestroyRenderer(renderer);
//             SDL_DestroyWindow(window);
//             SDL_Quit();
//             return 1;
//         }
//         obs.setSize(120, 120);
//         obstacles.push_back(obs);
//     }
//     float obstacleSpawnTimer = 0;

//     // Chargement autres assets
//     SDL_Texture* introBg = IMG_LoadTexture(renderer, "pictures/motobg.webp");
//     if (!introBg) {
//         std::cerr << "Failed to load intro background!" << std::endl;
//     }

//     SDL_Texture* menuBg = IMG_LoadTexture(renderer, "pictures/BGR.jpg");
//     if (!menuBg) {
//         std::cerr << "Failed to load menu background!" << std::endl;
//     }

//     SDL_Texture* gameover = nullptr;

//     // Création boutons menu
//     Button playButton(renderer, "pictures/play.png", 
//                      {SCREEN_WIDTH/2, SCREEN_HEIGHT/2, 450, 600});
//     Button exitButton(renderer, "pictures/quit.png", 
//                      {SCREEN_WIDTH/2, SCREEN_HEIGHT/2 + 100, 450, 600});
//     // Initialisation joueur
//     Player player;
//     if (!player.init(renderer, "pictures/b1.png")) {
//         std::cerr << "Failed to load player texture!" << std::endl;
//         SDL_DestroyRenderer(renderer);
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }
//     player.setSize(200, 200);
//     player.setGroundPosition(GROUND_HEIGHT);

//     // Boucle de jeu
//     GameState gameState;
//     float roadOffset = 0;
//     bool running = true;
//     SDL_Event e;
//     Uint32 lastTime = SDL_GetTicks();

//     while (running) {
//         float deltaTime = (SDL_GetTicks() - lastTime) / 1000.0f;
//         lastTime = SDL_GetTicks();

//         // Gestion événements
//         while (SDL_PollEvent(&e)) {
//             if (e.type == SDL_QUIT) {
//                 running = false;
//             }

//             if (gameState.inMenu) {
//                 if (e.type == SDL_MOUSEBUTTONDOWN) {
//                     int x, y;
//                     SDL_GetMouseState(&x, &y);

//                     if (playButton.isClicked(x, y)) {
//                         gameState.inMenu = false;
//                         gameState.inGame = true;
//                         player.reset();
//                         roadOffset = 0;
//                         for (auto& obs : obstacles) obs.reset();
//                     } else if (exitButton.isClicked(x, y)) {
//                         running = false;
//                     }
//                 }
//             }
//             else if (gameState.inGame) {
//                 player.handleEvent(e);
                
//                 if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
//                     gameState.inGame = false;
//                     gameState.inMenu = true;
//                 }
//             }
//         }

//         // Mise à jour jeu
//         if (gameState.showIntro && SDL_GetTicks() > 4000) {
//             gameState.showIntro = false;
//             gameState.inMenu = true;
//         }

//         if (gameState.inGame) {
//             roadOffset -= SCROLL_SPEED * deltaTime;
//             player.update(deltaTime);

//             // Gestion obstacles
//             obstacleSpawnTimer += deltaTime;
//             if (obstacleSpawnTimer >= OBSTACLE_SPAWN_INTERVAL) {
//                 obstacleSpawnTimer = 0;
//                 for (auto& obs : obstacles) {
//                     if (!obs.isActive()) {
//                         obs.spawn(SCREEN_WIDTH, GROUND_HEIGHT);
//                         break;
//                     }
//                 }
//             }

//             // Collisions
//             for (auto& obs : obstacles) {
//                 obs.update(deltaTime, SCROLL_SPEED);
                
//                 if (obs.isActive() && checkCollision(player.getCollider(), obs.getCollider())) {
//                     gameState.inGame = false;
//                     gameState.gameOver = true;
//                     gameState.gameOverStartTime = SDL_GetTicks();
//                     if (!gameover) {
//                         gameover = IMG_LoadTexture(renderer, "pictures/GAME OVER.png");
//                     }
//                 }
//             }
//         }

//         // Rendu
//         SDL_RenderClear(renderer);
        
//         if (gameState.showIntro) {
//             if (introBg) SDL_RenderCopy(renderer, introBg, nullptr, nullptr);
//         }
//         else if (gameState.inMenu) {
//             if (menuBg) SDL_RenderCopy(renderer, menuBg, nullptr, nullptr);
//             playButton.render(renderer);
//             exitButton.render(renderer);
//         }
//         else if (gameState.inGame) {
//             roadBg.render(renderer, roadOffset);
//             player.render(renderer);
//             for (auto& obs : obstacles) obs.render(renderer);
//         }
//         else if (gameState.gameOver) {
//             if (gameover) {
//                 SDL_RenderCopy(renderer, gameover, nullptr, nullptr);
//             }
            
//             // Retour au menu après 3 secondes
//             if (SDL_GetTicks() - gameState.gameOverStartTime > 5000) {
//                 gameState.gameOver = false;
//                 gameState.inMenu = true;
//                 SDL_DestroyTexture(gameover);
//                 gameover = nullptr;
//             }
//         }
        
//         SDL_RenderPresent(renderer);
//     }

//     // Nettoyage
//     if (gameover) SDL_DestroyTexture(gameover);
//     for (auto& obs : obstacles) obs.clean();
//     roadBg.clean();
//     if (introBg) SDL_DestroyTexture(introBg);
//     if (menuBg) SDL_DestroyTexture(menuBg);
//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     IMG_Quit();
//     SDL_Quit();

//     return 0;
// }

// #include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
// #include <iostream>
// #include <vector>
// #include "../headers/player.hpp"
// #include <cmath>
// #include "../headers/button.hpp"
// #include "../headers/Obstacle.hpp"

// const int SCREEN_WIDTH = 1000;
// const int SCREEN_HEIGHT = 700;
// const int GROUND_HEIGHT = SCREEN_HEIGHT - 10;
// const float OBSTACLE_SPAWN_INTERVAL = 2.0f;
// const float SCROLL_SPEED = 300.0f;

// struct GameState {
//     bool showIntro = true;
//     bool inMenu = false;
//     bool inGame = false;
//     bool gameOver = false;
//     Uint32 gameOverStartTime = 0;
// };

// class ScrollingBackground {
// public:
//     bool load(SDL_Renderer* renderer, const char* path) {
//         texture = IMG_LoadTexture(renderer, path);
//         if (!texture) {
//             std::cerr << "Failed to load background: " << IMG_GetError() << std::endl;
//             return false;
//         }
//         SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
//         return true;
//     }

//     void render(SDL_Renderer* renderer, float offset) {
//         float wrappedOffset = fmod(offset, width);
//         SDL_Rect dest1 = {static_cast<int>(wrappedOffset), 0, width, height};
//         SDL_RenderCopy(renderer, texture, nullptr, &dest1);
        
//         if (wrappedOffset > 0) {
//             SDL_Rect dest2 = {static_cast<int>(wrappedOffset) - width, 0, width, height};
//             SDL_RenderCopy(renderer, texture, nullptr, &dest2);
//         } else {
//             SDL_Rect dest2 = {static_cast<int>(wrappedOffset) + width, 0, width, height};
//             SDL_RenderCopy(renderer, texture, nullptr, &dest2);
//         }
//     }

//     void clean() {
//         if (texture) SDL_DestroyTexture(texture);
//     }

// private:
//     SDL_Texture* texture = nullptr;
//     int width = 0, height = 0;
// };

// bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
//     return (a.x < b.x + b.w &&
//             a.x + a.w > b.x &&
//             a.y < b.y + b.h &&
//             a.y + a.h > b.y);
// }

// int main(int argc, char* argv[]) {
//     // Initialisation SDL
//     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//         std::cerr << "SDL init error: " << SDL_GetError() << std::endl;
//         return 1;
//     }

//     if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
//         std::cerr << "SDL_image init error: " << IMG_GetError() << std::endl;
//         SDL_Quit();
//         return 1;
//     }

//     // Création fenêtre et renderer
//     SDL_Window* window = SDL_CreateWindow("Bike Racing Game",
//                                         SDL_WINDOWPOS_CENTERED,
//                                         SDL_WINDOWPOS_CENTERED,
//                                         SCREEN_WIDTH, SCREEN_HEIGHT,
//                                         SDL_WINDOW_SHOWN);
//     if (!window) {
//         std::cerr << "Window creation error: " << SDL_GetError() << std::endl;
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
//                                               SDL_RENDERER_ACCELERATED | 
//                                               SDL_RENDERER_PRESENTVSYNC);
//     if (!renderer) {
//         std::cerr << "Renderer creation error: " << SDL_GetError() << std::endl;
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }

//     // Chargement des assets
//     ScrollingBackground roadBg;
//     if (!roadBg.load(renderer, "pictures/background.png")) {
//         SDL_DestroyRenderer(renderer);
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Texture* introBg = IMG_LoadTexture(renderer, "pictures/motobg.webp");
//     SDL_Texture* menuBg = IMG_LoadTexture(renderer, "pictures/BGR.jpg");
//     SDL_Texture* gameover = nullptr;

//     // Création des boutons avec vos dimensions exactes
//     Button playButton(renderer, "pictures/play.png", 
//                     {SCREEN_WIDTH/2 - 225,  // x = centre - moitié de la largeur
//                      SCREEN_HEIGHT/2 - 205, // y = centre - moitié de la hauteur
//                      450,                   // width
//                      600});                 // height

//     Button exitButton(renderer, "pictures/quit.png", 
//                     {SCREEN_WIDTH/2 - 225,  // même x que playButton
//                      SCREEN_HEIGHT/2 + 100, // y = sous le playButton
//                      450, 
//                      600});

//     // Initialisation joueur
//     Player player;
//     if (!player.init(renderer, "pictures/b1.png")) {
//         std::cerr << "Failed to load player texture!" << std::endl;
//         SDL_DestroyRenderer(renderer);
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }
//     player.setSize(200, 200);
//     player.setGroundPosition(GROUND_HEIGHT);

//     // Initialisation obstacles
//     std::vector<Obstacle> obstacles;
//     for (int i = 0; i < 40; ++i) {
//         Obstacle obs;
//         if (!obs.load(renderer, "pictures/box4.png")) {
//             std::cerr << "Failed to load obstacle texture!" << std::endl;
//             SDL_DestroyRenderer(renderer);
//             SDL_DestroyWindow(window);
//             SDL_Quit();
//             return 1;
//         }
//         obs.setSize(120, 120);
//         obstacles.push_back(obs);
//     }
//     float obstacleSpawnTimer = 0;

//     // Boucle de jeu
//     GameState gameState;
//     float roadOffset = 0;
//     bool running = true;
//     SDL_Event e;
//     Uint32 lastTime = SDL_GetTicks();

//     while (running) {
//         float deltaTime = (SDL_GetTicks() - lastTime) / 1000.0f;
//         lastTime = SDL_GetTicks();

//         // Gestion événements
//         while (SDL_PollEvent(&e)) {
//             if (e.type == SDL_QUIT) {
//                 running = false;
//             }

//             if (gameState.inMenu) {
//                 if (e.type == SDL_MOUSEBUTTONDOWN) {
//                     int x, y;
//                     SDL_GetMouseState(&x, &y);

//                     if (playButton.isClicked(x, y)) {
//                         gameState.inMenu = false;
//                         gameState.inGame = true;
//                         player.reset();
//                         roadOffset = 0;
//                         for (auto& obs : obstacles) obs.reset();
//                     } 
//                     else if (exitButton.isClicked(x, y)) {
//                         running = false; // Quitter le jeu
//                     }
//                 }
//             }
//             else if (gameState.inGame) {
//                 player.handleEvent(e);
                
//                 if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
//                     gameState.inGame = false;
//                     gameState.inMenu = true;
//                 }
//             }
//         }

//         // Mise à jour jeu
//         if (gameState.showIntro && SDL_GetTicks() > 4000) {
//             gameState.showIntro = false;
//             gameState.inMenu = true;
//         }

//         if (gameState.inGame) {
//             roadOffset -= SCROLL_SPEED * deltaTime;
//             player.update(deltaTime);

//             // Gestion obstacles
//             obstacleSpawnTimer += deltaTime;
//             if (obstacleSpawnTimer >= OBSTACLE_SPAWN_INTERVAL) {
//                 obstacleSpawnTimer = 0;
//                 for (auto& obs : obstacles) {
//                     if (!obs.isActive()) {
//                         obs.spawn(SCREEN_WIDTH, GROUND_HEIGHT);
//                         break;
//                     }
//                 }
//             }

//             // Collisions
//             for (auto& obs : obstacles) {
//                 obs.update(deltaTime, SCROLL_SPEED);
                
//                 if (obs.isActive() && checkCollision(player.getCollider(), obs.getCollider())) {
//                     gameState.inGame = false;
//                     gameState.gameOver = true;
//                     gameState.gameOverStartTime = SDL_GetTicks();
//                     if (!gameover) {
//                         gameover = IMG_LoadTexture(renderer, "pictures/GAME OVER.png");
//                     }
//                 }
//             }
//         }

//         // Rendu
//         SDL_RenderClear(renderer);
        
//         if (gameState.showIntro) {
//             if (introBg) SDL_RenderCopy(renderer, introBg, nullptr, nullptr);
//         }
//         else if (gameState.inMenu) {
//             if (menuBg) SDL_RenderCopy(renderer, menuBg, nullptr, nullptr);
//             playButton.render(renderer);
//             exitButton.render(renderer);
//         }
//         else if (gameState.inGame) {
//             roadBg.render(renderer, roadOffset);
//             player.render(renderer);
//             for (auto& obs : obstacles) obs.render(renderer);
//         }
//         else if (gameState.gameOver) {
//             if (gameover) {
//                 SDL_RenderCopy(renderer, gameover, nullptr, nullptr);
//             }
            
//             // Retour au menu après 3 secondes
//             if (SDL_GetTicks() - gameState.gameOverStartTime > 3000) {
//                 gameState.gameOver = false;
//                 gameState.inMenu = true;
//                 SDL_DestroyTexture(gameover);
//                 gameover = nullptr;
//             }
//         }
        
//         SDL_RenderPresent(renderer);
//     }

//     // Nettoyage
//     if (gameover) SDL_DestroyTexture(gameover);
//     for (auto& obs : obstacles) obs.clean();
//     roadBg.clean();
//     if (introBg) SDL_DestroyTexture(introBg);
//     if (menuBg) SDL_DestroyTexture(menuBg);
//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     IMG_Quit();
//     SDL_Quit();

//     return 0;
// }











// #include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
// #include <iostream>
// #include <vector>
// #include "../headers/player.hpp"
// #include <cmath>
// #include "../headers/button.hpp"
// #include "../headers/Obstacle.hpp"

// const int SCREEN_WIDTH = 1000;
// const int SCREEN_HEIGHT = 700;
// const int GROUND_HEIGHT = SCREEN_HEIGHT - 10;
// const float OBSTACLE_SPAWN_INTERVAL = 2.0f;
// const float SCROLL_SPEED = 300.0f;
// const int FINISH_LINE_POSITION = 1000; // Position X de la ligne d'arrivée

// struct GameState {
//     bool showIntro = true;
//     bool inMenu = false;
//     bool inGame = false;
//     bool gameOver = false;
//     bool gameWon = false;
//     Uint32 gameOverStartTime = 0;
// };

// class ScrollingBackground {
// public:
//     bool load(SDL_Renderer* renderer, const char* path) {
//         texture = IMG_LoadTexture(renderer, path);
//         if (!texture) {
//             std::cerr << "Failed to load background: " << IMG_GetError() << std::endl;
//             return false;
//         }
//         SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
//         return true;
//     }

//     void render(SDL_Renderer* renderer, float offset) {
//         float wrappedOffset = fmod(offset, width);
//         SDL_Rect dest1 = {static_cast<int>(wrappedOffset), 0, width, height};
//         SDL_RenderCopy(renderer, texture, nullptr, &dest1);
        
//         if (wrappedOffset > 0) {
//             SDL_Rect dest2 = {static_cast<int>(wrappedOffset) - width, 0, width, height};
//             SDL_RenderCopy(renderer, texture, nullptr, &dest2);
//         } else {
//             SDL_Rect dest2 = {static_cast<int>(wrappedOffset) + width, 0, width, height};
//             SDL_RenderCopy(renderer, texture, nullptr, &dest2);
//         }
//     }

//     void clean() {
//         if (texture) SDL_DestroyTexture(texture);
//     }

// private:
//     SDL_Texture* texture = nullptr;
//     int width = 0, height = 0;
// };

// bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
//     return (a.x < b.x + b.w &&
//             a.x + a.w > b.x &&
//             a.y < b.y + b.h &&
//             a.y + a.h > b.y);
// }




// int main(int argc, char* argv[]) {
//     // Initialisation SDL
//     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//         std::cerr << "SDL init error: " << SDL_GetError() << std::endl;
//         return 1;
//     }

//     if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
//         std::cerr << "SDL_image init error: " << IMG_GetError() << std::endl;
//         SDL_Quit();
//         return 1;
//     }

//     // Création fenêtre et renderer
//     SDL_Window* window = SDL_CreateWindow("Bike Racing Game",
//                                         SDL_WINDOWPOS_CENTERED,
//                                         SDL_WINDOWPOS_CENTERED,
//                                         SCREEN_WIDTH, SCREEN_HEIGHT,
//                                         SDL_WINDOW_SHOWN);
//     if (!window) {
//         std::cerr << "Window creation error: " << SDL_GetError() << std::endl;
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
//                                               SDL_RENDERER_ACCELERATED | 
//                                               SDL_RENDERER_PRESENTVSYNC);
//     if (!renderer) {
//         std::cerr << "Renderer creation error: " << SDL_GetError() << std::endl;
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }

//     // Chargement des assets
//     ScrollingBackground roadBg;
//     if (!roadBg.load(renderer, "pictures/background.png")) {
//         SDL_DestroyRenderer(renderer);
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Texture* introBg = IMG_LoadTexture(renderer, "pictures/motobg.webp");
//     SDL_Texture* menuBg = IMG_LoadTexture(renderer, "pictures/BGR.jpg");
//     SDL_Texture* gameover = nullptr;
//     // Chargement de la texture pour la ligne d'arrivée
//     SDL_Texture* finishLineTexture = IMG_LoadTexture(renderer, "pictures/finish.png");
//     if (!finishLineTexture) {
//         std::cerr << "Failed to load finish line texture!" << std::endl;
//         // Gérer l'erreur...
//     }


//     // Position de la ligne d'arrivée
//     SDL_Rect finishLineRect = {FINISH_LINE_POSITION, GROUND_HEIGHT - 150, 50, 160}; // Position de la ligne d'arrivée (X, Y, largeur, hauteur)

//     // Création des boutons avec vos dimensions exactes
//     Button playButton(renderer, "pictures/play.png", 
//                     {SCREEN_WIDTH/2 - 225,  // x = centre - moitié de la largeur
//                      SCREEN_HEIGHT/2 - 205, // y = centre - moitié de la hauteur
//                      450,                   // width
//                      600});                 // height

//     Button exitButton(renderer, "pictures/quit.png", 
//                     {SCREEN_WIDTH/2 - 225,  // même x que playButton
//                      SCREEN_HEIGHT/2 + 100, // y = sous le playButton
//                      450, 
//                      600});

//     // Initialisation joueur
//     Player player;
//     if (!player.init(renderer, "pictures/b1.png")) {
//         std::cerr << "Failed to load player texture!" << std::endl;
//         SDL_DestroyRenderer(renderer);
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }
//     player.setSize(200, 200);
//     player.setGroundPosition(GROUND_HEIGHT);

//     // Initialisation obstacles
//     std::vector<Obstacle> obstacles;
//     for (int i = 0; i < 40; ++i) {
//         Obstacle obs;
//         if (!obs.load(renderer, "pictures/box4.png")) {
//             std::cerr << "Failed to load obstacle texture!" << std::endl;
//             SDL_DestroyRenderer(renderer);
//             SDL_DestroyWindow(window);
//             SDL_Quit();
//             return 1;
//         }
//         obs.setSize(120, 120);
//         obstacles.push_back(obs);
//     }
//     float obstacleSpawnTimer = 0;

//     // Boucle de jeu
//     GameState gameState;
//     float roadOffset = 0;
//     bool running = true;
//     SDL_Event e;
//     Uint32 lastTime = SDL_GetTicks();

//     while (running) {
//         float deltaTime = (SDL_GetTicks() - lastTime) / 1000.0f;
//         lastTime = SDL_GetTicks();

//         // Gestion événements
//         while (SDL_PollEvent(&e)) {
//             if (e.type == SDL_QUIT) {
//                 running = false;
//             }

//             if (gameState.inMenu) {
//                 if (e.type == SDL_MOUSEBUTTONDOWN) {
//                     int x, y;
//                     SDL_GetMouseState(&x, &y);

//                     if (playButton.isClicked(x, y)) {
//                         gameState.inMenu = false;
//                         gameState.inGame = true;
//                         player.reset();
//                         roadOffset = 0;
//                         for (auto& obs : obstacles) obs.reset();
//                     } 
//                     else if (exitButton.isClicked(x, y)) {
//                         running = false; // Quitter le jeu
//                     }
//                 }
//             }
//             else if (gameState.inGame) {
//                 player.handleEvent(e);
                
//                 if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
//                     gameState.inGame = false;
//                     gameState.inMenu = true;
//                 }
//             }
//         }

//         // Mise à jour jeu
//         if (gameState.showIntro && SDL_GetTicks() > 4000) {
//             gameState.showIntro = false;
//             gameState.inMenu = true;
//         }

//         if (gameState.inGame) {
//             roadOffset -= SCROLL_SPEED * deltaTime;
//             player.update(deltaTime);

//             // Vérifier si le joueur a atteint la ligne d'arrivée
//             if (player.getPositionX() >= FINISH_LINE_POSITION && !gameState.gameWon) {
//                 gameState.inGame = false;
//                 gameState.gameWon = true;
//                 gameState.gameOverStartTime = SDL_GetTicks();
//                 // Charger une texture de victoire si nécessaire
//             }

//             // Gestion obstacles
//             obstacleSpawnTimer += deltaTime;
//             if (obstacleSpawnTimer >= OBSTACLE_SPAWN_INTERVAL) {
//                 obstacleSpawnTimer = 0;
//                 for (auto& obs : obstacles) {
//                     if (!obs.isActive()) {
//                         obs.spawn(SCREEN_WIDTH, GROUND_HEIGHT);
//                         break;
//                     }
//                 }
//             }

//             // Collisions
//             for (auto& obs : obstacles) {
//                 obs.update(deltaTime, SCROLL_SPEED);
                
//                 if (obs.isActive() && checkCollision(player.getCollider(), obs.getCollider())) {
//                     gameState.inGame = false;
//                     gameState.gameOver = true;
//                     gameState.gameOverStartTime = SDL_GetTicks();
//                     if (!gameover) {
//                         gameover = IMG_LoadTexture(renderer, "pictures/GAME OVER.png");
//                     }
//                 }
//             }
//         }

//         // Rendu
//         SDL_RenderClear(renderer);
        
//         if (gameState.showIntro) {
//             if (introBg) SDL_RenderCopy(renderer, introBg, nullptr, nullptr);
//         }
//         else if (gameState.inMenu) {
//             if (menuBg) SDL_RenderCopy(renderer, menuBg, nullptr, nullptr);
//             playButton.render(renderer);
//             exitButton.render(renderer);
//         }
//         else if (gameState.inGame) {
//             roadBg.render(renderer, roadOffset);

//             // Dessiner la ligne d'arrivée
//             if (finishLineTexture) {
//                 SDL_Rect finishLineOnScreen = {
//                     static_cast<int>(finishLineRect.x - player.getPositionX()),
//                     finishLineRect.y,
//                     finishLineRect.w,
//                     finishLineRect.h
//                 };
//                 SDL_RenderCopy(renderer, finishLineTexture, nullptr, &finishLineOnScreen);
//             }

//             player.render(renderer);
//             for (auto& obs : obstacles) obs.render(renderer);
//         }
//         else if (gameState.gameOver) {
//             if (gameover) {
//                 SDL_RenderCopy(renderer, gameover, nullptr, nullptr);
//             }
            
//             // Retour au menu après 3 secondes
//             if (SDL_GetTicks() - gameState.gameOverStartTime > 3000) {
//                 gameState.gameOver = false;
//                 gameState.inMenu = true;
//                 SDL_DestroyTexture(gameover);
//                 gameover = nullptr;
//             }
//         }
//         else if (gameState.gameWon) {
//             // Afficher un écran de victoire
//             SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
//             SDL_RenderClear(renderer);

//             // Vous pouvez ajouter du texte ou une image de victoire ici

//             // Retour au menu après 3 secondes
//             if (SDL_GetTicks() - gameState.gameOverStartTime > 3000) {
//                 gameState.gameWon = false;
//                 gameState.inMenu = true;
//             }
//         }
        
//         SDL_RenderPresent(renderer);
//     }

//     // Nettoyage
//     if (gameover) SDL_DestroyTexture(gameover);
//     if (finishLineTexture) SDL_DestroyTexture(finishLineTexture);
//     for (auto& obs : obstacles) obs.clean();
//     roadBg.clean();
//     if (introBg) SDL_DestroyTexture(introBg);
//     if (menuBg) SDL_DestroyTexture(menuBg);
//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     IMG_Quit();
//     SDL_Quit();

//     return 0;
// }










// #include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
// #include <iostream>
// #include <vector>
// #include "../headers/player.hpp"
// #include <cmath>
// #include "../headers/button.hpp"
// #include "../headers/Obstacle.hpp"

// const int SCREEN_WIDTH = 1000;
// const int SCREEN_HEIGHT = 700;
// const int GROUND_HEIGHT = SCREEN_HEIGHT - 10;
// const float OBSTACLE_SPAWN_INTERVAL = 2.0f;
// const float SCROLL_SPEED = 300.0f;
// const int FINISH_LINE_WORLD_X = 1000; // Distance player needs to travel to reach finish

// struct GameState {
//     bool showIntro = true;
//     bool inMenu = false;
//     bool inGame = false;
//     bool gameOver = false;
//     bool gameWon = false;
//     Uint32 gameOverStartTime = 0;
// };

// class ScrollingBackground {
// public:
//     bool load(SDL_Renderer* renderer, const char* path) {
//         texture = IMG_LoadTexture(renderer, path);
//         if (!texture) {
//             std::cerr << "Failed to load background: " << IMG_GetError() << std::endl;
//             return false;
//         }
//         SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
//         return true;
//     }

//     void render(SDL_Renderer* renderer, float offset) {
//         float wrappedOffset = fmod(offset, width);
//         SDL_Rect dest1 = {static_cast<int>(wrappedOffset), 0, width, height};
//         SDL_RenderCopy(renderer, texture, nullptr, &dest1);
        
//         if (wrappedOffset > 0) {
//             SDL_Rect dest2 = {static_cast<int>(wrappedOffset) - width, 0, width, height};
//             SDL_RenderCopy(renderer, texture, nullptr, &dest2);
//         } else {
//             SDL_Rect dest2 = {static_cast<int>(wrappedOffset) + width, 0, width, height};
//             SDL_RenderCopy(renderer, texture, nullptr, &dest2);
//         }
//     }

//     void clean() {
//         if (texture) SDL_DestroyTexture(texture);
//     }

// private:
//     SDL_Texture* texture = nullptr;
//     int width = 0, height = 0;
// };

// bool checkCollision(const SDL_Rect& a, const SDL_Rect& b) {
//     return (a.x < b.x + b.w &&
//             a.x + a.w > b.x &&
//             a.y < b.y + b.h &&
//             a.y + a.h > b.y);
// }

// int main(int argc, char* argv[]) {
//     // Initialisation SDL
//     if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//         std::cerr << "SDL init error: " << SDL_GetError() << std::endl;
//         return 1;
//     }

//     if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
//         std::cerr << "SDL_image init error: " << IMG_GetError() << std::endl;
//         SDL_Quit();
//         return 1;
//     }

//     // Create window and renderer
//     SDL_Window* window = SDL_CreateWindow("Bike Racing Game",
//                                         SDL_WINDOWPOS_CENTERED,
//                                         SDL_WINDOWPOS_CENTERED,
//                                         SCREEN_WIDTH, SCREEN_HEIGHT,
//                                         SDL_WINDOW_SHOWN);
//     if (!window) {
//         std::cerr << "Window creation error: " << SDL_GetError() << std::endl;
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, 
//                                               SDL_RENDERER_ACCELERATED | 
//                                               SDL_RENDERER_PRESENTVSYNC);
//     if (!renderer) {
//         std::cerr << "Renderer creation error: " << SDL_GetError() << std::endl;
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }

//     // Load assets
//     ScrollingBackground roadBg;
//     if (!roadBg.load(renderer, "pictures/background.png")) {
//         SDL_DestroyRenderer(renderer);
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }

//     SDL_Texture* introBg = IMG_LoadTexture(renderer, "pictures/motobg.webp");
//     SDL_Texture* menuBg = IMG_LoadTexture(renderer, "pictures/BGR.jpg");
//     SDL_Texture* gameover = nullptr;
//     SDL_Texture* victoryTexture = IMG_LoadTexture(renderer, "pictures/victory.png");
//     SDL_Texture* finishLineTexture = IMG_LoadTexture(renderer, "pictures/finish.png");
//     if (!finishLineTexture) {
//         std::cerr << "Failed to load finish line texture!" << std::endl;
//     }

//     // Create buttons
//     Button playButton(renderer, "pictures/play.png", 
//                     {SCREEN_WIDTH/2 - 225,
//                      SCREEN_HEIGHT/2 - 205,
//                      450,
//                      600});

//     Button exitButton(renderer, "pictures/quit.png", 
//                     {SCREEN_WIDTH/2 - 225,
//                      SCREEN_HEIGHT/2 + 100,
//                      450, 
//                      600});

//     // Initialize player
//     Player player;
//     if (!player.init(renderer, "pictures/b1.png")) {
//         std::cerr << "Failed to load player texture!" << std::endl;
//         SDL_DestroyRenderer(renderer);
//         SDL_DestroyWindow(window);
//         SDL_Quit();
//         return 1;
//     }
//     player.setSize(200, 200);
//     player.setGroundPosition(GROUND_HEIGHT);

//     // Initialize obstacles
//     std::vector<Obstacle> obstacles;
//     for (int i = 0; i < 40; ++i) {
//         Obstacle obs;
//         if (!obs.load(renderer, "pictures/box4.png")) {
//             std::cerr << "Failed to load obstacle texture!" << std::endl;
//             SDL_DestroyRenderer(renderer);
//             SDL_DestroyWindow(window);
//             SDL_Quit();
//             return 1;
//         }
//         obs.setSize(120, 120);
//         obstacles.push_back(obs);
//     }
//     float obstacleSpawnTimer = 0;

//     // Game loop
//     GameState gameState;
//     float roadOffset = 0;
//     bool running = true;
//     SDL_Event e;
//     Uint32 lastTime = SDL_GetTicks();

//     while (running) {
//         float deltaTime = (SDL_GetTicks() - lastTime) / 1000.0f;
//         lastTime = SDL_GetTicks();

//         // Event handling
//         while (SDL_PollEvent(&e)) {
//             if (e.type == SDL_QUIT) {
//                 running = false;
//             }

//             if (gameState.inMenu) {
//                 if (e.type == SDL_MOUSEBUTTONDOWN) {
//                     int x, y;
//                     SDL_GetMouseState(&x, &y);

//                     if (playButton.isClicked(x, y)) {
//                         gameState.inMenu = false;
//                         gameState.inGame = true;
//                         player.reset();
//                         roadOffset = 0;
//                         for (auto& obs : obstacles) obs.reset();
//                     } 
//                     else if (exitButton.isClicked(x, y)) {
//                         running = false;
//                     }
//                 }
//             }
//             else if (gameState.inGame) {
//                 player.handleEvent(e);
                
//                 if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
//                     gameState.inGame = false;
//                     gameState.inMenu = true;
//                 }
//             }
//         }

//         // Game updates
//         if (gameState.showIntro && SDL_GetTicks() > 4000) {
//             gameState.showIntro = false;
//             gameState.inMenu = true;
//         }

//         if (gameState.inGame) {
//             roadOffset -= SCROLL_SPEED * deltaTime;
//             player.update(deltaTime);

//             // Check if player reached finish line
//             if (player.getPositionX() >= FINISH_LINE_WORLD_X && !gameState.gameWon) {
//                 gameState.inGame = false;
//                 gameState.gameWon = true;
//                 gameState.gameOverStartTime = SDL_GetTicks();
//             }

//             // Obstacle spawning
//             obstacleSpawnTimer += deltaTime;
//             if (obstacleSpawnTimer >= OBSTACLE_SPAWN_INTERVAL) {
//                 obstacleSpawnTimer = 0;
//                 for (auto& obs : obstacles) {
//                     if (!obs.isActive()) {
//                         obs.spawn(SCREEN_WIDTH, GROUND_HEIGHT);
//                         break;
//                     }
//                 }
//             }

//             // Collisions
//             for (auto& obs : obstacles) {
//                 obs.update(deltaTime, SCROLL_SPEED);
                
//                 if (obs.isActive() && checkCollision(player.getCollider(), obs.getCollider())) {
//                     gameState.inGame = false;
//                     gameState.gameOver = true;
//                     gameState.gameOverStartTime = SDL_GetTicks();
//                     if (!gameover) {
//                         gameover = IMG_LoadTexture(renderer, "pictures/GAME OVER.png");
//                     }
//                 }
//             }
//         }

//         // Rendering
//         SDL_RenderClear(renderer);
        
//         if (gameState.showIntro) {
//             if (introBg) SDL_RenderCopy(renderer, introBg, nullptr, nullptr);
//         }
//         else if (gameState.inMenu) {
//             if (menuBg) SDL_RenderCopy(renderer, menuBg, nullptr, nullptr);
//             playButton.render(renderer);
//             exitButton.render(renderer);
//         }
//         else if (gameState.inGame) {
//             roadBg.render(renderer, roadOffset);
            
//             // Draw finish line - now properly synced with background scrolling
//             if (finishLineTexture) {
//                 // Calculate screen position based on world position and scroll
//                 int finishLineScreenX = FINISH_LINE_WORLD_X + static_cast<int>(roadOffset);
                
//                 // Only draw when visible on screen
//                 if (finishLineScreenX > -100 && finishLineScreenX < SCREEN_WIDTH) {
//                     SDL_Rect finishLineRect = {
//                         finishLineScreenX,
//                         GROUND_HEIGHT - 150, // Adjust height as needed
//                         50,                   // Width
//                         160                   // Height
//                     };
//                     SDL_RenderCopy(renderer, finishLineTexture, nullptr, &finishLineRect);
//                 }
//             }
            
//             player.render(renderer);
//             for (auto& obs : obstacles) obs.render(renderer);
//         }
//         else if (gameState.gameOver) {
//             if (gameover) {
//                 SDL_RenderCopy(renderer, gameover, nullptr, nullptr);
//             }
            
//             if (SDL_GetTicks() - gameState.gameOverStartTime > 3000) {
//                 gameState.gameOver = false;
//                 gameState.inMenu = true;
//                 SDL_DestroyTexture(gameover);
//                 gameover = nullptr;
//             }
//         }
//         else if (gameState.gameWon) {
//             if (victoryTexture) {
//                 SDL_RenderCopy(renderer, victoryTexture, nullptr, nullptr);
//             } else {
//                 // Fallback victory screen
//                 SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
//                 SDL_RenderClear(renderer);
//             }
            
//             if (SDL_GetTicks() - gameState.gameOverStartTime > 3000) {
//                 gameState.gameWon = false;
//                 gameState.inMenu = true;
//             }
//         }
        
//         SDL_RenderPresent(renderer);
//     }

//     // Cleanup
//     if (gameover) SDL_DestroyTexture(gameover);
//     if (victoryTexture) SDL_DestroyTexture(victoryTexture);
//     if (finishLineTexture) SDL_DestroyTexture(finishLineTexture);
//     for (auto& obs : obstacles) obs.clean();
//     roadBg.clean();
//     if (introBg) SDL_DestroyTexture(introBg);
//     if (menuBg) SDL_DestroyTexture(menuBg);
//     SDL_DestroyRenderer(renderer);
//     SDL_DestroyWindow(window);
//     IMG_Quit();
//     SDL_Quit();

//     return 0;
// }


#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <cmath>
#include "../headers/player.hpp"
#include "../headers/button.hpp"
#include "../headers/Obstacle.hpp"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 700;
const int GROUND_HEIGHT = SCREEN_HEIGHT - 10;
const float OBSTACLE_SPAWN_INTERVAL = 2.0f;
const float SCROLL_SPEED = 300.0f;
const int FINISH_LINE_WORLD_X = 2000;

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

    if (!finishLineTexture) {
        std::cerr << "Failed to load finish line texture!" << std::endl;
    }

    SDL_Rect finishLineRect = {0, GROUND_HEIGHT - 150, 50, 160};

    Button playButton(renderer, "pictures/play.png",
                      {SCREEN_WIDTH/2 - 225, SCREEN_HEIGHT/2 - 205, 450, 600});
    Button exitButton(renderer, "pictures/quit.png",
                      {SCREEN_WIDTH/2 - 225, SCREEN_HEIGHT/2 + 100, 450, 600});

    Player player;
    if (!player.init(renderer, "pictures/b1.png")) {
        std::cerr << "Failed to load player texture!" << std::endl;
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
            std::cerr << "Failed to load obstacle texture!" << std::endl;
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
        obs.setSize(120, 120);
        obstacles.push_back(obs);
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

            // Update finish line rect based on scrolling
            finishLineRect.x = FINISH_LINE_WORLD_X + static_cast<int>(roadOffset);

            // Check win by collision
            if (!gameState.gameWon && checkCollision(player.getCollider(), finishLineRect)) {
                gameState.inGame = false;
                gameState.gameWon = true;
                gameState.gameOverStartTime = SDL_GetTicks();
                std::cout << "Victory! Finish line touched." << std::endl;
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
    IMG_Quit();
    SDL_Quit();

    return 0;
}
