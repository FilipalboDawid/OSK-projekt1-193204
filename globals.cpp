#include "globals.hpp"

int columns = 8;
int rows = 8;
int minesCount = 10;
int flagsPlaced = 0;
bool isFirstClick = true;
sf::Clock gameClock;
int elapsedTime = 0;

float logicalW = 650.0f;
float logicalH = 550.0f;
bool isFullscreen = false;

std::vector<Cell> grid;
GameState currentState = GameState::Menu;
std::string activeSkin = "classic"; 

int selectedDifficulty = 0; 
std::string strCustomCols = "10";
std::string strCustomRows = "10";
std::string strCustomMines = "15";
int activeInputField = 0; 
std::string playerName = "";
bool isDropdownOpen = false; 

sf::Texture texHidden, texEmpty, texMine, texFlag;
sf::Texture texNumbers[8];
sf::Font font;
std::vector<ScoreEntry> leaderboards[4];