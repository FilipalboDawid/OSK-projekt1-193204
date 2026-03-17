#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

const int TILE_SIZE = 32;
const int TOP_UI_HEIGHT = 50; 
const int MIN_WINDOW_WIDTH = 400;

struct Cell {
    bool isMine = false;
    bool isRevealed = false;
    bool isFlagged = false;
    int adjacentMines = 0;
};

struct ScoreEntry {
    std::string name;
    int time;
};

enum class GameState { Menu, Playing, GameOver, Victory, EnterName, Leaderboard };

// Słowo 'extern' informuje, że zmienne te są zdefiniowane w pliku .cpp
extern int columns, rows, minesCount, flagsPlaced, elapsedTime;
extern bool isFirstClick, isFullscreen, isDropdownOpen;
extern float logicalW, logicalH;
extern sf::Clock gameClock;

extern std::vector<Cell> grid;
extern GameState currentState;
extern std::string activeSkin;

extern int selectedDifficulty, customCols, customRows, customMines, activeInputField;
extern std::string strCustomCols, strCustomRows, strCustomMines, playerName;

extern sf::Texture texHidden, texEmpty, texMine, texFlag;
extern sf::Texture texNumbers[8];
extern sf::Font font;
extern std::vector<ScoreEntry> leaderboards[4];

// --- ELEMENTY UI ---
extern sf::Text titleMenu, btnBeginner, btnIntermediate, btnExpert, btnCustom;
extern sf::Text lblCols, valCol, lblRows, valRow, lblMines, valMine;
extern sf::Text btnSkinClassic, btnSkinModern, btnSkinGreen, btnStart, btnShowLeaderboard;
extern sf::Text msgEnd, txtTimer, txtMines, btnOptions;
extern sf::Text dropRestart, dropSkin, dropFullscreen, dropMenu;
extern sf::RectangleShape dropdownBg, gameBg;
extern sf::Text txtTitle, txtSubtitle, btnReturnMenu;