# Makefile for Bike Racing Game using SDL2
# This Makefile is used to compile the Bike Racing Game project using g++
all:
	g++ -std=c++17 fichiers/main.cpp fichiers/player.cpp fichiers/button.cpp  fichiers/obstacle.cpp fichiers/score.cpp  -o moto_game `sdl2-config --cflags --libs` -lSDL2_image -lSDL2_ttf


