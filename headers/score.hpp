#ifndef SCORE_HPP
#define SCORE_HPP

#include <string>
#include <vector>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

// Structure pour stocker un score
struct ScoreEntry {
    std::string playerName;
    int score;
    float time;
    
    ScoreEntry(const std::string& name, int scr, float t) 
        : playerName(name), score(scr), time(t) {}
};

class ScoreManager {
public:
    static const int MAX_SCORES = 10;
    static const char* SCORES_FILE;

    ScoreManager();
    ~ScoreManager();

    // Gestion des scores
    void addScore(const std::string& playerName, int score, float time);
    std::vector<ScoreEntry> getHighScores() const;
    void saveScores() const;
    void loadScores();

    // Affichage
    void renderScores(SDL_Renderer* renderer, TTF_Font* font, int x, int y) const;
    void renderScore(SDL_Renderer* renderer, TTF_Font* font, int score, int x, int y) const;

    // Vérification si un score est dans les highscores
    bool isHighScore(int score) const;

private:
    std::vector<ScoreEntry> highScores;

    void sortScores();
};

#endif // SCORE_HPP