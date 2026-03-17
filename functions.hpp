#pragma once
#include <SFML/Graphics.hpp>
#include <string>

int getIndex(int x, int y);
bool isValid(int x, int y);
bool loadTextures(const std::string& skinName);
void loadLeaderboard();
void saveLeaderboard();
void addScore(int diff, std::string name, int time);
void adjustView(sf::RenderWindow& window);
void toggleFullscreen(sf::RenderWindow& window);
void applyWindowSize(sf::RenderWindow& window, unsigned int w, unsigned int h);
void clampCustomSettings();

void initGame();
void generateMines(int safeX, int safeY); // NOWA
void saveState();                         // NOWA (do cofania)
void undoState();                         // NOWA (do cofania)
void revealCell(int x, int y);
void chordCell(int x, int y);             // NOWA (bezpieczna okolica)
void openRemainingSafeCells();            // NOWA (klik w licznik)
void checkVictory();
void revealAllMines();
void cycleSkin();