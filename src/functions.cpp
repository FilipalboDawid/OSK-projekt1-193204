#include "functions.hpp"
#include "globals.hpp"
#include <iostream>
#include <random>
#include <algorithm>
#include <fstream>

int getIndex(int x, int y) { return y * columns + x; }
bool isValid(int x, int y) { return x >= 0 && x < columns && y >= 0 && y < rows; }

bool loadTextures(const std::string& skinName) {
    std::string path = "assets/" + skinName + "/";
    if (!texHidden.loadFromFile(path + "hidden.png") ||
        !texEmpty.loadFromFile(path + "empty.png") ||
        !texMine.loadFromFile(path + "mine.png") ||
        !texFlag.loadFromFile(path + "flag.png") ||
        !texQuestion.loadFromFile(path + "question.png")) {
        std::cerr << "Blad: Nie znaleziono tekstur w " << path << "\n";
        return false;
    }
    for (int i = 0; i < 8; ++i) {
        if (!texNumbers[i].loadFromFile(path + std::to_string(i + 1) + ".png")) return false;
    }
    return true;
}

void loadLeaderboard() {
    for (int i = 0; i < 4; ++i) leaderboards[i].clear();
    std::ifstream file("assets/leaderboard.txt");
    if (file.is_open()) {
        int diff, time; std::string name;
        while (file >> diff >> time >> name) {
            if (diff >= 0 && diff < 4) leaderboards[diff].push_back({name, time});
        }
        file.close();
    }
    for (int i = 0; i < 4; ++i) {
        std::sort(leaderboards[i].begin(), leaderboards[i].end(), [](const ScoreEntry& a, const ScoreEntry& b) {
            return a.time < b.time;
        });
    }
}

void saveLeaderboard() {
    std::ofstream file("assets/leaderboard.txt");
    if (file.is_open()) {
        for (int i = 0; i < 4; ++i) {
            int limit = std::min((int)leaderboards[i].size(), 10);
            for (int j = 0; j < limit; ++j) {
                file << i << " " << leaderboards[i][j].time << " " << leaderboards[i][j].name << "\n";
            }
        }
        file.close();
    }
}

void addScore(int diff, std::string name, int time) {
    if (name.empty()) name = "Anonim";
    std::replace(name.begin(), name.end(), ' ', '_'); 
    leaderboards[diff].push_back({name, time});
    saveLeaderboard();
    loadLeaderboard(); 
}

void adjustView(sf::RenderWindow& window) {
    float windowRatio = window.getSize().x / (float)window.getSize().y;
    float viewRatio = logicalW / logicalH;
    float sizeX = 1.0f, sizeY = 1.0f, posX = 0.0f, posY = 0.0f;
    if (windowRatio >= viewRatio) {
        sizeX = viewRatio / windowRatio; posX = (1.0f - sizeX) / 2.0f;
    } else {
        sizeY = windowRatio / viewRatio; posY = (1.0f - sizeY) / 2.0f;
    }
    sf::View view(sf::FloatRect(0.0f, 0.0f, logicalW, logicalH));
    view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
    window.setView(view);
}

void toggleFullscreen(sf::RenderWindow& window) {
    isFullscreen = !isFullscreen;
    if (isFullscreen) window.create(sf::VideoMode::getDesktopMode(), "Saper", sf::Style::Fullscreen);
    else window.create(sf::VideoMode(logicalW, logicalH), "Saper", sf::Style::Default);
    window.setFramerateLimit(60);
    adjustView(window);
}

void applyWindowSize(sf::RenderWindow& window, unsigned int w, unsigned int h) {
    logicalW = static_cast<float>(w);
    logicalH = static_cast<float>(h);
    if (!isFullscreen) window.setSize(sf::Vector2u(w, h));
    adjustView(window);
}

void clampCustomSettings() {
    int c = strCustomCols.empty() ? 8 : std::stoi(strCustomCols);
    int r = strCustomRows.empty() ? 8 : std::stoi(strCustomRows);
    int m = strCustomMines.empty() ? 10 : std::stoi(strCustomMines);
    if (c < 8) c = 8; if (c > 30) c = 30;
    if (r < 8) r = 8; if (r > 24) r = 24;
    int maxMines = (c * r) / 3;
    if (maxMines > 240) maxMines = 240;
    if (m < 10) m = 10; if (m > maxMines) m = maxMines;
    strCustomCols = std::to_string(c); strCustomRows = std::to_string(r); strCustomMines = std::to_string(m);
    columns = c; rows = r; minesCount = m;
}

void initGame() {
    grid.assign(columns * rows, Cell());
    previousGrid = grid;
    currentState = GameState::Playing;
    flagsPlaced = 0;
    prevFlagsPlaced = 0;
    isFirstClick = true;
    elapsedTime = 0;
    isDropdownOpen = false; 
}

void saveState() {
    if (!optUndo) return;
    previousGrid = grid;
    prevFlagsPlaced = flagsPlaced;
}

void undoState() {
    if (!optUndo || previousGrid.empty()) return;
    grid = previousGrid;
    flagsPlaced = prevFlagsPlaced;
    currentState = GameState::Playing;
}

void generateMines(int safeX, int safeY) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(0, columns - 1);
    std::uniform_int_distribution<> disY(0, rows - 1);

    int placed = 0;
    while (placed < minesCount) {
        int rx = disX(gen);
        int ry = disY(gen);
        int idx = getIndex(rx, ry);

        bool isSafe = false;
        if (rx == safeX && ry == safeY) isSafe = true;
        if (optOpeningMove && std::abs(rx - safeX) <= 1 && std::abs(ry - safeY) <= 1) isSafe = true;

        if (!grid[idx].isMine && !isSafe) {
            grid[idx].isMine = true;
            placed++;
        }
    }

    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            if (grid[getIndex(x, y)].isMine) continue;
            int count = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (isValid(x + dx, y + dy) && grid[getIndex(x + dx, y + dy)].isMine) count++;
                }
            }
            grid[getIndex(x, y)].adjacentMines = count;
        }
    }
}

void revealCell(int x, int y) {
    if (!isValid(x, y)) return;
    int idx = getIndex(x, y);
    if (grid[idx].isRevealed || grid[idx].flagState != 0) return;

    grid[idx].isRevealed = true;

    if (grid[idx].adjacentMines == 0 && !grid[idx].isMine) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx != 0 || dy != 0) revealCell(x + dx, y + dy);
            }
        }
    }
}

void chordCell(int x, int y) {
    int idx = getIndex(x, y);
    if (!grid[idx].isRevealed || grid[idx].adjacentMines == 0) return;

    int nearbyFlags = 0;
    for (int dy = -1; dy <= 1; ++dy) {
        for (int dx = -1; dx <= 1; ++dx) {
            if (isValid(x + dx, y + dy) && grid[getIndex(x + dx, y + dy)].flagState == 1) {
                nearbyFlags++;
            }
        }
    }

    if (nearbyFlags == grid[idx].adjacentMines) {
        saveState();
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (isValid(x + dx, y + dy)) {
                    int neighborIdx = getIndex(x + dx, y + dy);
                    if (!grid[neighborIdx].isRevealed && grid[neighborIdx].flagState == 0) {
                        if (grid[neighborIdx].isMine) {
                            currentState = GameState::GameOver;
                            revealAllMines();
                        } else {
                            revealCell(x + dx, y + dy);
                        }
                    }
                }
            }
        }
        if (currentState != GameState::GameOver) checkVictory();
    }
}

void openRemainingSafeCells() {
    if (minesCount - flagsPlaced != 0) return;
    saveState();
    for (int y = 0; y < rows; ++y) {
        for (int x = 0; x < columns; ++x) {
            int idx = getIndex(x, y);
            if (!grid[idx].isRevealed && grid[idx].flagState != 1) {
                if (grid[idx].isMine) {
                    currentState = GameState::GameOver;
                    revealAllMines();
                } else {
                    revealCell(x, y);
                }
            }
        }
    }
    if (currentState != GameState::GameOver) checkVictory();
}

void checkVictory() {
    for (const auto& cell : grid) {
        if (!cell.isMine && !cell.isRevealed) return;
    }
    currentState = GameState::EnterName;
    playerName = ""; 
}

void revealAllMines() {
    for (auto& cell : grid) {
        if (cell.isMine) cell.isRevealed = true;
    }
}

void setSkin(const std::string& skinName) {
    activeSkin = skinName;
    loadTextures(activeSkin);
}

void clearLeaderboard() {
    for (int i = 0; i < 4; ++i) {
        leaderboards[i].clear();
    }
    saveLeaderboard();
}