#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <algorithm>
#include <fstream> 

const int TILE_SIZE = 32;
const int TOP_UI_HEIGHT = 50; 
const int MIN_WINDOW_WIDTH = 400; // Minimalna szerokość, żeby UI się nie zlało

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
std::vector<ScoreEntry> leaderboards[4]; 

enum class GameState { Menu, Playing, GameOver, Victory, EnterName, Leaderboard };

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

int getIndex(int x, int y) { return y * columns + x; }
bool isValid(int x, int y) { return x >= 0 && x < columns && y >= 0 && y < rows; }

bool loadTextures(const std::string& skinName) {
    std::string path = "assets/" + skinName + "/";
    if (!texHidden.loadFromFile(path + "hidden.png") ||
        !texEmpty.loadFromFile(path + "empty.png") ||
        !texMine.loadFromFile(path + "mine.png") ||
        !texFlag.loadFromFile(path + "flag.png")) {
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
    std::ifstream file("leaderboard.txt");
    if (file.is_open()) {
        int diff, time;
        std::string name;
        while (file >> diff >> time >> name) {
            if (diff >= 0 && diff < 4) {
                leaderboards[diff].push_back({name, time});
            }
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
    std::ofstream file("leaderboard.txt");
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
        sizeX = viewRatio / windowRatio;
        posX = (1.0f - sizeX) / 2.0f;
    } else {
        sizeY = windowRatio / viewRatio;
        posY = (1.0f - sizeY) / 2.0f;
    }

    sf::View view(sf::FloatRect(0.0f, 0.0f, logicalW, logicalH));
    view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
    window.setView(view);
}

void toggleFullscreen(sf::RenderWindow& window) {
    isFullscreen = !isFullscreen;
    if (isFullscreen) window.create(sf::VideoMode::getDesktopMode(), "Minesweeper C++", sf::Style::Fullscreen);
    else window.create(sf::VideoMode(logicalW, logicalH), "Minesweeper C++", sf::Style::Default);
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

    if (m < 10) m = 10;
    if (m > maxMines) m = maxMines;

    strCustomCols = std::to_string(c);
    strCustomRows = std::to_string(r);
    strCustomMines = std::to_string(m);
    
    columns = c; rows = r; minesCount = m;
}

void initGame() {
    grid.assign(columns * rows, Cell());
    currentState = GameState::Playing;
    flagsPlaced = 0;
    isFirstClick = true;
    elapsedTime = 0;
    isDropdownOpen = false; 

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(0, columns - 1);
    std::uniform_int_distribution<> disY(0, rows - 1);

    int minesPlaced = 0;
    while (minesPlaced < minesCount) {
        int x = disX(gen);
        int y = disY(gen);
        int idx = getIndex(x, y);

        if (!grid[idx].isMine) {
            grid[idx].isMine = true;
            minesPlaced++;
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
    if (grid[idx].isRevealed || grid[idx].isFlagged) return;

    grid[idx].isRevealed = true;

    if (grid[idx].adjacentMines == 0 && !grid[idx].isMine) {
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx != 0 || dy != 0) revealCell(x + dx, y + dy);
            }
        }
    }
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

void cycleSkin() {
    if (activeSkin == "classic") activeSkin = "modern";
    else if (activeSkin == "modern") activeSkin = "green";
    else activeSkin = "classic";
    loadTextures(activeSkin);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(logicalW, logicalH), "Minesweeper C++", sf::Style::Default);
    window.setFramerateLimit(60);

    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }

    loadTextures(activeSkin);
    loadLeaderboard(); 

    sf::Text titleMenu("USTAWIENIA GRY", font, 30); titleMenu.setPosition(20.0f, 20.0f);
    
    sf::Text btnBeginner("[ Poczatkujacy (8x8, 10 min) ]", font, 20);      btnBeginner.setPosition(20.0f, 70.0f);
    sf::Text btnIntermediate("[ Zaawansowany (16x16, 40 min) ]", font, 20); btnIntermediate.setPosition(20.0f, 100.0f);
    sf::Text btnExpert("[ Ekspert (30x16, 99 min) ]", font, 20);            btnExpert.setPosition(20.0f, 130.0f);
    sf::Text btnCustom("[ Plansza Uzytkownika ]", font, 20);                btnCustom.setPosition(20.0f, 160.0f);

    sf::Text lblCols("Szerokosc (8-30):", font, 18); lblCols.setPosition(50.0f, 190.0f);
    sf::Text valCol("10", font, 20);                 valCol.setPosition(250.0f, 188.0f);

    sf::Text lblRows("Wysokosc (8-24):", font, 18);  lblRows.setPosition(50.0f, 220.0f);
    sf::Text valRow("10", font, 20);                 valRow.setPosition(250.0f, 218.0f);

    sf::Text lblMines("Miny (max A*B/3):", font, 18); lblMines.setPosition(50.0f, 250.0f);
    sf::Text valMine("15", font, 20);                 valMine.setPosition(250.0f, 248.0f);

    sf::Text btnSkinClassic("[ Skorka: Classic ]", font, 20); btnSkinClassic.setPosition(20.0f, 320.0f);
    sf::Text btnSkinModern("[ Skorka: Modern ]", font, 20);   btnSkinModern.setPosition(220.0f, 320.0f);
    sf::Text btnSkinGreen("[ Skorka: Green ]", font, 20);     btnSkinGreen.setPosition(420.0f, 320.0f);
    sf::Text btnStart(">>> START GRY <<<", font, 32);         btnStart.setPosition(20.0f, 380.0f);
    
    sf::Text btnShowLeaderboard("[ TABELA WYNIKOW ]", font, 20); btnShowLeaderboard.setPosition(420.0f, 390.0f);

    sf::Text msgEnd("", font, 20); 
    sf::Text txtTimer("", font, 20);
    sf::Text txtMines("", font, 20);
    sf::Text btnOptions("[ Opcje ]", font, 20); btnOptions.setPosition(10.0f, 12.0f);

    sf::Text dropRestart("Restart", font, 18);   dropRestart.setPosition(15.0f, TOP_UI_HEIGHT + 10.0f);
    sf::Text dropSkin("Zmien Skorke", font, 18); dropSkin.setPosition(15.0f, TOP_UI_HEIGHT + 40.0f);
    sf::Text dropFullscreen("Pelny Ekran (F11)", font, 18); dropFullscreen.setPosition(15.0f, TOP_UI_HEIGHT + 70.0f);
    sf::Text dropMenu("Wroc do Menu", font, 18); dropMenu.setPosition(15.0f, TOP_UI_HEIGHT + 100.0f);
    
    sf::RectangleShape dropdownBg(sf::Vector2f(200.0f, 130.0f));
    dropdownBg.setFillColor(sf::Color(50, 50, 50, 240)); 
    dropdownBg.setPosition(10.0f, TOP_UI_HEIGHT);

    sf::RectangleShape gameBg;

    sf::Text txtTitle("", font, 40); txtTitle.setPosition(50.0f, 50.0f);
    sf::Text txtSubtitle("", font, 24); txtSubtitle.setPosition(50.0f, 120.0f);
    sf::Text btnReturnMenu("[ Wroc do Menu ]", font, 24); btnReturnMenu.setPosition(50.0f, 480.0f);

    auto styleButton = [](sf::Text& btn, bool isSelected, sf::Vector2f mousePos) {
        if (isSelected) btn.setFillColor(sf::Color(0, 255, 0));
        else if (btn.getGlobalBounds().contains(mousePos)) btn.setFillColor(sf::Color(255, 255, 0));
        else btn.setFillColor(sf::Color(255, 255, 255));
    };

    while (window.isOpen()) {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

        // --- KALKULACJE SZEROKOŚCI I WYSRODKOWANIA PLANSZY ---
        float boardWidth = columns * TILE_SIZE;
        float boardHeight = rows * TILE_SIZE;
        // Obliczamy offset (przesunięcie w prawo), jeśli okno jest szersze niż plansza (np. na 8x8)
        float offsetX = (logicalW > boardWidth) ? (logicalW - boardWidth) / 2.0f : 0.0f;

        if (currentState == GameState::Playing && !isFirstClick) {
            elapsedTime = static_cast<int>(gameClock.getElapsedTime().asSeconds());
        }

        if (currentState == GameState::Menu) {
            styleButton(btnBeginner, selectedDifficulty == 0, mousePos);
            styleButton(btnIntermediate, selectedDifficulty == 1, mousePos);
            styleButton(btnExpert, selectedDifficulty == 2, mousePos);
            styleButton(btnCustom, selectedDifficulty == 3, mousePos);

            valCol.setFillColor(activeInputField == 1 ? sf::Color::Red : sf::Color::White);
            valRow.setFillColor(activeInputField == 2 ? sf::Color::Red : sf::Color::White);
            valMine.setFillColor(activeInputField == 3 ? sf::Color::Red : sf::Color::White);

            styleButton(btnSkinClassic, activeSkin == "classic", mousePos);
            styleButton(btnSkinModern, activeSkin == "modern", mousePos);
            styleButton(btnSkinGreen, activeSkin == "green", mousePos);
            styleButton(btnStart, false, mousePos);
            styleButton(btnShowLeaderboard, false, mousePos);

            valCol.setString(strCustomCols + (activeInputField == 1 ? "_" : ""));
            valRow.setString(strCustomRows + (activeInputField == 2 ? "_" : ""));
            valMine.setString(strCustomMines + (activeInputField == 3 ? "_" : ""));
        } 
        else if (currentState == GameState::Playing || currentState == GameState::GameOver) {
            styleButton(btnOptions, isDropdownOpen, mousePos); 
            styleButton(msgEnd, false, mousePos);
            if (isDropdownOpen) {
                styleButton(dropRestart, false, mousePos);
                styleButton(dropSkin, false, mousePos);
                styleButton(dropFullscreen, false, mousePos);
                styleButton(dropMenu, false, mousePos);
            }
        }
        else if (currentState == GameState::EnterName || currentState == GameState::Leaderboard) {
            styleButton(btnReturnMenu, false, mousePos);
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::Resized) adjustView(window);
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11) toggleFullscreen(window);

            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) { 
                    char c = static_cast<char>(event.text.unicode);
                    
                    if (currentState == GameState::EnterName) {
                        if (c == '\b' && !playerName.empty()) playerName.pop_back();
                        else if (c == '\r' || c == '\n') { 
                            addScore(selectedDifficulty, playerName, elapsedTime);
                            currentState = GameState::Leaderboard;
                        }
                        else if (std::isprint(c) && playerName.size() < 15) playerName += c;
                    }
                    else if (currentState == GameState::Menu && selectedDifficulty == 3) {
                        std::string* activeStr = nullptr;
                        if (activeInputField == 1) activeStr = &strCustomCols;
                        if (activeInputField == 2) activeStr = &strCustomRows;
                        if (activeInputField == 3) activeStr = &strCustomMines;

                        if (activeStr) {
                            if (c == '\b' && !activeStr->empty()) activeStr->pop_back();
                            else if (std::isdigit(c) && activeStr->size() < 3) *activeStr += c;
                        }
                    }
                }
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    
                    if (currentState == GameState::Menu) {
                        activeInputField = 0; 
                        
                        if (btnBeginner.getGlobalBounds().contains(mousePos)) selectedDifficulty = 0;
                        else if (btnIntermediate.getGlobalBounds().contains(mousePos)) selectedDifficulty = 1;
                        else if (btnExpert.getGlobalBounds().contains(mousePos)) selectedDifficulty = 2;
                        else if (btnCustom.getGlobalBounds().contains(mousePos)) selectedDifficulty = 3;
                        
                        if (selectedDifficulty == 3) {
                            if (valCol.getGlobalBounds().contains(mousePos)) activeInputField = 1;
                            else if (valRow.getGlobalBounds().contains(mousePos)) activeInputField = 2;
                            else if (valMine.getGlobalBounds().contains(mousePos)) activeInputField = 3;
                        }

                        if (btnSkinClassic.getGlobalBounds().contains(mousePos)) { activeSkin = "classic"; loadTextures(activeSkin); }
                        else if (btnSkinModern.getGlobalBounds().contains(mousePos)) { activeSkin = "modern"; loadTextures(activeSkin); }
                        else if (btnSkinGreen.getGlobalBounds().contains(mousePos)) { activeSkin = "green"; loadTextures(activeSkin); }
                        else if (btnShowLeaderboard.getGlobalBounds().contains(mousePos)) currentState = GameState::Leaderboard;
                        else if (btnStart.getGlobalBounds().contains(mousePos)) {
                            
                            if (selectedDifficulty == 0) { columns = 8; rows = 8; minesCount = 10; }
                            else if (selectedDifficulty == 1) { columns = 16; rows = 16; minesCount = 40; }
                            else if (selectedDifficulty == 2) { columns = 30; rows = 16; minesCount = 99; }
                            else { clampCustomSettings(); } 

                            // UWAGA: Minimalna szerokość na poziomie gry to 400px!
                            unsigned int w = std::max((unsigned int)MIN_WINDOW_WIDTH, (unsigned int)(columns * TILE_SIZE)); 
                            unsigned int h = rows * TILE_SIZE + TOP_UI_HEIGHT;
                            applyWindowSize(window, w, h);
                            initGame();
                        }
                    }
                    else if (currentState == GameState::Playing) {
                        if (isDropdownOpen) {
                            if (dropRestart.getGlobalBounds().contains(mousePos)) initGame();
                            else if (dropSkin.getGlobalBounds().contains(mousePos)) cycleSkin();
                            else if (dropFullscreen.getGlobalBounds().contains(mousePos)) toggleFullscreen(window);
                            else if (dropMenu.getGlobalBounds().contains(mousePos)) {
                                currentState = GameState::Menu;
                                applyWindowSize(window, 650, 550); 
                            }
                            isDropdownOpen = false; 
                        }
                        else if (btnOptions.getGlobalBounds().contains(mousePos)) isDropdownOpen = true;
                        else {
                            // Kliknięcie w planszę z uwzględnieniem offsetu
                            if (mousePos.x >= offsetX && mousePos.x < offsetX + boardWidth && mousePos.y >= TOP_UI_HEIGHT) {
                                int x = static_cast<int>(mousePos.x - offsetX) / TILE_SIZE;
                                int y = (static_cast<int>(mousePos.y) - TOP_UI_HEIGHT) / TILE_SIZE;
                                if (isValid(x, y) && !grid[getIndex(x, y)].isFlagged) {
                                    if (isFirstClick) {
                                        isFirstClick = false;
                                        gameClock.restart();
                                    }
                                    if (grid[getIndex(x, y)].isMine) {
                                        currentState = GameState::GameOver;
                                        revealAllMines();
                                    } else {
                                        revealCell(x, y);
                                        checkVictory();
                                    }
                                }
                            }
                        }
                    }
                    else if (currentState == GameState::GameOver) {
                        if (isDropdownOpen) {
                            if (dropRestart.getGlobalBounds().contains(mousePos)) initGame();
                            else if (dropSkin.getGlobalBounds().contains(mousePos)) cycleSkin();
                            else if (dropFullscreen.getGlobalBounds().contains(mousePos)) toggleFullscreen(window);
                            else if (dropMenu.getGlobalBounds().contains(mousePos)) {
                                currentState = GameState::Menu;
                                applyWindowSize(window, 650, 550);
                            }
                            isDropdownOpen = false;
                        }
                        else if (btnOptions.getGlobalBounds().contains(mousePos)) isDropdownOpen = true;
                        else if (!btnOptions.getGlobalBounds().contains(mousePos)) initGame();
                    }
                    else if (currentState == GameState::EnterName || currentState == GameState::Leaderboard) {
                        if (btnReturnMenu.getGlobalBounds().contains(mousePos)) {
                            currentState = GameState::Menu;
                            applyWindowSize(window, 650, 550);
                        }
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Right && currentState == GameState::Playing && !isDropdownOpen) {
                    if (mousePos.x >= offsetX && mousePos.x < offsetX + boardWidth && mousePos.y >= TOP_UI_HEIGHT) {
                        int x = static_cast<int>(mousePos.x - offsetX) / TILE_SIZE;
                        int y = (static_cast<int>(mousePos.y) - TOP_UI_HEIGHT) / TILE_SIZE;
                        if (isValid(x, y) && !grid[getIndex(x, y)].isRevealed) {
                            grid[getIndex(x, y)].isFlagged = !grid[getIndex(x, y)].isFlagged;
                            if (grid[getIndex(x, y)].isFlagged) flagsPlaced++;
                            else flagsPlaced--;
                        }
                    }
                }
            }
        }

        window.clear(sf::Color::Black);
        gameBg.setSize(sf::Vector2f(logicalW, logicalH));
        gameBg.setFillColor(sf::Color(70, 70, 70));
        window.draw(gameBg);

        if (currentState == GameState::Menu) {
            window.draw(titleMenu);
            window.draw(btnBeginner); window.draw(btnIntermediate); window.draw(btnExpert); window.draw(btnCustom);
            
            if (selectedDifficulty == 3) {
                window.draw(lblCols); window.draw(valCol);
                window.draw(lblRows); window.draw(valRow);
                window.draw(lblMines); window.draw(valMine);
            }

            window.draw(btnSkinClassic); window.draw(btnSkinModern); window.draw(btnSkinGreen);
            window.draw(btnStart);
            window.draw(btnShowLeaderboard);
        } 
        else if (currentState == GameState::Playing || currentState == GameState::GameOver) {
            // Rysowanie kafelków przesuniętych o offsetX
            for (int y = 0; y < rows; ++y) {
                for (int x = 0; x < columns; ++x) {
                    Cell cell = grid[getIndex(x, y)];
                    const sf::Texture* currentTex = &texHidden;
                    
                    if (!cell.isRevealed) {
                        if (cell.isFlagged) currentTex = &texFlag;
                    } else {
                        if (cell.isMine) currentTex = &texMine;
                        else if (cell.adjacentMines == 0) currentTex = &texEmpty;
                        else currentTex = &texNumbers[cell.adjacentMines - 1];
                    }

                    sf::Sprite sprite;
                    sprite.setTexture(*currentTex);
                    sprite.setPosition(static_cast<float>(x * TILE_SIZE) + offsetX, static_cast<float>(y * TILE_SIZE + TOP_UI_HEIGHT));
                    window.draw(sprite);
                }
            }

            // Pasek na całą dostępną szerokość okna
            sf::RectangleShape topBar(sf::Vector2f(logicalW, TOP_UI_HEIGHT * 1.0f));
            topBar.setFillColor(sf::Color(40, 40, 40));
            window.draw(topBar);
            
            window.draw(btnOptions); 

            txtTimer.setString("Czas: " + std::to_string(elapsedTime) + "s");
            txtTimer.setPosition(logicalW / 2.0f - 40.0f, 12.0f);
            window.draw(txtTimer);

            txtMines.setString("Miny: " + std::to_string(minesCount - flagsPlaced));
            txtMines.setPosition(logicalW - 120.0f, 12.0f);
            window.draw(txtMines);

            if (currentState == GameState::GameOver) {
                sf::RectangleShape overlay(sf::Vector2f(logicalW, 60.0f));
                overlay.setFillColor(sf::Color(0, 0, 0, 200));
                overlay.setPosition(0.0f, TOP_UI_HEIGHT + (rows * TILE_SIZE) / 2.0f - 30.0f);
                window.draw(overlay);

                msgEnd.setString("PRZEGRANA! Kliknij by zagrac");
                msgEnd.setPosition(logicalW / 2.0f - msgEnd.getGlobalBounds().width / 2.0f, TOP_UI_HEIGHT + (rows * TILE_SIZE) / 2.0f - 15.0f);
                window.draw(msgEnd);
            }

            if (isDropdownOpen) {
                window.draw(dropdownBg);
                window.draw(dropRestart); window.draw(dropSkin); 
                window.draw(dropFullscreen); window.draw(dropMenu);
            }
        }
        else if (currentState == GameState::EnterName) {
            applyWindowSize(window, 650, 550); 
            txtTitle.setString("WYGRANA!");
            txtTitle.setFillColor(sf::Color::Green);
            txtSubtitle.setString("Czas: " + std::to_string(elapsedTime) + "s\nWpisz swoje imie (i nacisnij ENTER):\n\n> " + playerName + "_");
            window.draw(txtTitle);
            window.draw(txtSubtitle);
            window.draw(btnReturnMenu);
        }
        else if (currentState == GameState::Leaderboard) {
            applyWindowSize(window, 650, 550);
            txtTitle.setString("TABELA WYNIKOW");
            txtTitle.setFillColor(sf::Color::White);
            window.draw(txtTitle);

            std::string diffNames[] = {"Poczatkujacy", "Zaawansowany", "Ekspert", "Wlasna Plansza"};
            for (int i = 0; i < 4; ++i) {
                sf::Text header(diffNames[i], font, 20);
                header.setFillColor(sf::Color::Yellow);
                header.setPosition(50.0f, 120.0f + i * 90.0f);
                window.draw(header);

                std::string scoresText = "";
                int limit = std::min((int)leaderboards[i].size(), 3); 
                if (limit == 0) scoresText = "Brak wynikow.";
                for (int j = 0; j < limit; ++j) {
                    scoresText += std::to_string(j+1) + ". " + leaderboards[i][j].name + " - " + std::to_string(leaderboards[i][j].time) + "s\n";
                }
                sf::Text scoresList(scoresText, font, 16);
                scoresList.setPosition(50.0f, 150.0f + i * 90.0f);
                window.draw(scoresList);
            }
            window.draw(btnReturnMenu);
        }

        window.display();
    }

    return 0;
}