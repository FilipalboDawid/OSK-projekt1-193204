#include "globals.hpp"

int columns = 8;
int rows = 8;
int minesCount = 10;
int flagsPlaced = 0;
bool isFirstClick = true;
sf::Clock gameClock;
int elapsedTime = 0;

float logicalW = 750.0f; // Minimalnie poszerzone menu, zeby zmiescic nowe opcje po prawej
float logicalH = 550.0f;
bool isFullscreen = false;

std::vector<Cell> grid;
std::vector<Cell> previousGrid;
int prevFlagsPlaced = 0;

GameState currentState = GameState::Menu;
std::string activeSkin = "classic"; 

// Dodatkowe opcje (domyślnie wyłączone)
bool optOpeningMove = false;
bool optQuestionMarks = false;
bool optChording = false;
bool optOpenRemaining = false;
bool optUndo = false;
bool optHints = false;

int selectedDifficulty = 0; 
std::string strCustomCols = "10";
std::string strCustomRows = "10";
std::string strCustomMines = "15";
int activeInputField = 0; 
std::string playerName = "";
bool isDropdownOpen = false; 

sf::Texture texHidden, texEmpty, texMine, texFlag, texQuestion;
sf::Texture texNumbers[8];
sf::Font font;
std::vector<ScoreEntry> leaderboards[4];

// --- ELEMENTY UI ---
sf::Text titleMenu, btnBeginner, btnIntermediate, btnExpert, btnCustom;
sf::Text lblCols, valCol, lblRows, valRow, lblMines, valMine;
sf::Text btnSkinClassic, btnSkinModern, btnSkinGreen, btnStart, btnShowLeaderboard;
sf::Text msgEnd, txtTimer, txtMines, btnOptions;
sf::Text dropRestart, dropUndo, dropSkin, dropFullscreen, dropMenu;
sf::RectangleShape dropdownBg, gameBg;
sf::Text txtTitle, txtSubtitle, btnReturnMenu;

sf::Text lblOptionsTitle;
sf::Text btnOptOpening, btnOptQuestion, btnOptChording, btnOptRemaining, btnOptUndo, btnOptHints;