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
    int flagState = 0; 
    int adjacentMines = 0;
};

struct ScoreEntry {
    std::string name;
    int time;
};

enum class GameState { Menu, Playing, GameOver, Victory, EnterName, Leaderboard };

extern int columns, rows, minesCount, flagsPlaced, elapsedTime;
extern bool isFirstClick, isFullscreen, isDropdownOpen, isSkinDropdownOpen; 
extern float logicalW, logicalH;
extern sf::Clock gameClock;

extern std::vector<Cell> grid;
extern std::vector<Cell> previousGrid; 
extern int prevFlagsPlaced;

extern GameState currentState;
extern std::string activeSkin;

extern bool optOpeningMove, optQuestionMarks, optChording, optOpenRemaining, optUndo, optHints;

extern int selectedDifficulty, customCols, customRows, customMines, activeInputField;
extern std::string strCustomCols, strCustomRows, strCustomMines, playerName;

extern sf::Texture texHidden, texEmpty, texMine, texFlag, texQuestion;
extern sf::Texture texNumbers[8];
extern sf::Font font;
extern std::vector<ScoreEntry> leaderboards[4];

// --- ELEMENTY UI ---
extern sf::Text titleMenu, btnBeginner, btnIntermediate, btnExpert, btnCustom;
extern sf::Text lblCols, valCol, lblRows, valRow, lblMines, valMine;
extern sf::Text btnStart, btnShowLeaderboard, btnQuit;
extern sf::Text msgEnd, txtTimer, txtMines, btnOptions;
extern sf::Text dropRestart, dropUndo, dropSkin, dropFullscreen, dropMenu, dropQuit;
extern sf::RectangleShape dropdownBg, gameBg;
extern sf::Text txtTitle, txtSubtitle, btnReturnMenu;

extern sf::Text lblOptionsTitle;
extern sf::Text btnOptOpening, btnOptQuestion, btnOptChording, btnOptRemaining, btnOptUndo, btnOptHints;

extern sf::Text tooltipText;
extern sf::RectangleShape tooltipBg;
extern bool showTooltip;

extern sf::Text btnSkinSelect, optSkinClassic, optSkinModern, optSkinGreen;
extern sf::RectangleShape bgSkinDropdown;