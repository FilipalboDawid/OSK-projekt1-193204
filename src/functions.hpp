#pragma once
#include <SFML/Graphics.hpp>
#include <string>

int getIndex(int x, int y);
bool isValid(int x, int y);
bool loadTextures(const std::string& skinName);
void loadLeaderboard();
void saveLeaderboard();
void clearLeaderboard(); // DODANO
void addScore(int diff, std::string name, int time);
void adjustView(sf::RenderWindow& window);
void toggleFullscreen(sf::RenderWindow& window);
void applyWindowSize(sf::RenderWindow& window, unsigned int w, unsigned int h);
void clampCustomSettings();

void initGame();
void generateMines(int safeX, int safeY); 
void saveState();                         
void undoState();                         
void revealCell(int x, int y);
void chordCell(int x, int y);             
void openRemainingSafeCells();            
void checkVictory();
void revealAllMines();
void setSkin(const std::string& skinName); // ZMIENIONO!