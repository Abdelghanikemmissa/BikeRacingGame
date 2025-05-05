#include "../headers/score.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>

const char* ScoreManager::SCORES_FILE = "scores.txt";

ScoreManager::ScoreManager() {
    loadScores();
}

ScoreManager::~ScoreManager() {
    saveScores();
}

void ScoreManager::addScore(const std::string& playerName, int score, float time) {
    highScores.emplace_back(playerName, score, time);
    sortScores();
    
    // Garder seulement les meilleurs scores
    if (highScores.size() > MAX_SCORES) {
        highScores.resize(MAX_SCORES);
    }
    
    saveScores();
}

std::vector<ScoreEntry> ScoreManager::getHighScores() const {
    return highScores;
}

void ScoreManager::saveScores() const {
    std::ofstream file(SCORES_FILE);
    if (file.is_open()) {
        for (const auto& entry : highScores) {
            file << entry.playerName << "," 
                 << entry.score << "," 
                 << entry.time << "\n";
        }
    } else {
        std::cerr << "Failed to save scores to " << SCORES_FILE << std::endl;
    }
}

void ScoreManager::loadScores() {
    highScores.clear();
    std::ifstream file(SCORES_FILE);
    
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            std::string name;
            std::string scoreStr;
            std::string timeStr;
            
            if (std::getline(iss, name, ',') && 
                std::getline(iss, scoreStr, ',') &&
                std::getline(iss, timeStr)) {
                try {
                    int score = std::stoi(scoreStr);
                    float time = std::stof(timeStr);
                    highScores.emplace_back(name, score, time);
                } catch (...) {
                    std::cerr << "Error parsing score entry: " << line << std::endl;
                }
            }
        }
        sortScores();
    }
}

void ScoreManager::sortScores() {
    std::sort(highScores.begin(), highScores.end(),
        [](const ScoreEntry& a, const ScoreEntry& b) {
            if (a.score != b.score) {
                return a.score > b.score; // D'abord par score décroissant
            }
            return a.time < b.time; // Ensuite par temps croissant
        });
}

void ScoreManager::renderScores(SDL_Renderer* renderer, TTF_Font* font, int x, int y) const {
    SDL_Color white = {255, 255, 255, 255};
    SDL_Color gold = {255, 215, 0, 255};
    SDL_Color silver = {192, 192, 192, 255};
    SDL_Color bronze = {205, 127, 50, 255};
    
    for (size_t i = 0; i < highScores.size(); ++i) {
        const auto& entry = highScores[i];
        SDL_Color color = white;
        
        // Couleurs spéciales pour les 3 premiers
        if (i == 0) color = gold;
        else if (i == 1) color = silver;
        else if (i == 2) color = bronze;
        
        std::stringstream ss;
        ss << (i+1) << ". " << entry.playerName << ": " 
           << entry.score << " pts (" << entry.time << "s)";
        
        SDL_Surface* surface = TTF_RenderText_Solid(font, ss.str().c_str(), color);
        if (surface) {
            SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (texture) {
                SDL_Rect rect = {x, y + static_cast<int>(i) * 30, surface->w, surface->h};
                SDL_RenderCopy(renderer, texture, nullptr, &rect);
                SDL_DestroyTexture(texture);
            }
            SDL_FreeSurface(surface);
        }
    }
}

void ScoreManager::renderScore(SDL_Renderer* renderer, TTF_Font* font, int score, int x, int y) const {
    SDL_Color white = {255, 255, 255, 255};
    std::string text = "Score: " + std::to_string(score);
    
    SDL_Surface* surface = TTF_RenderText_Solid(font, text.c_str(), white);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        if (texture) {
            SDL_Rect rect = {x, y, surface->w, surface->h};
            SDL_RenderCopy(renderer, texture, nullptr, &rect);
            SDL_DestroyTexture(texture);
        }
        SDL_FreeSurface(surface);
    }
}

bool ScoreManager::isHighScore(int score) const {
    if (highScores.size() < MAX_SCORES) return true;
    return score > highScores.back().score;
}