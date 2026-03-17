#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <random>
#include <algorithm>

const int TILE_SIZE = 32;
const int TOP_UI_HEIGHT = 50; 

int columns = 9;
int rows = 9;
int minesCount = 10;

int flagsPlaced = 0;
bool isFirstClick = true;
sf::Clock gameClock;
int elapsedTime = 0;

// Logiczny rozmiar gry (niezależny od fizycznego rozmiaru okna na monitorze)
float logicalW = 650.0f;
float logicalH = 350.0f;
bool isFullscreen = false;

struct Cell {
    bool isMine = false;
    bool isRevealed = false;
    bool isFlagged = false;
    int adjacentMines = 0;
};

enum class GameState { Menu, Playing, GameOver, Victory };

std::vector<Cell> grid;
GameState currentState = GameState::Menu;
std::string activeSkin = "classic"; 

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

// Funkcja dopasowująca widok (View) do rozmiaru okna, zachowując proporcje gry (czarne paski)
void adjustView(sf::RenderWindow& window) {
    float windowRatio = window.getSize().x / (float)window.getSize().y;
    float viewRatio = logicalW / logicalH;
    float sizeX = 1.0f;
    float sizeY = 1.0f;
    float posX = 0.0f;
    float posY = 0.0f;

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

// Funkcja zmieniająca tryb ekranu
void toggleFullscreen(sf::RenderWindow& window) {
    isFullscreen = !isFullscreen;
    if (isFullscreen) {
        window.create(sf::VideoMode::getDesktopMode(), "Minesweeper C++", sf::Style::Fullscreen);
    } else {
        window.create(sf::VideoMode(logicalW, logicalH), "Minesweeper C++", sf::Style::Default);
    }
    window.setFramerateLimit(60);
    adjustView(window);
}

void applyWindowSize(sf::RenderWindow& window, unsigned int w, unsigned int h) {
    logicalW = static_cast<float>(w);
    logicalH = static_cast<float>(h);
    if (!isFullscreen) {
        window.setSize(sf::Vector2u(w, h));
    }
    adjustView(window);
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
    currentState = GameState::Victory;
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

    sf::Text titleMenu("USTAWIENIA GRY", font, 30); titleMenu.setPosition(20.0f, 20.0f);
    sf::Text btnEasy("[ Latwy (9x9) ]", font, 20);      btnEasy.setPosition(20.0f, 80.0f);
    sf::Text btnMedium("[ Sredni (16x16) ]", font, 20); btnMedium.setPosition(200.0f, 80.0f);
    sf::Text btnHard("[ Trudny (30x16) ]", font, 20);   btnHard.setPosition(420.0f, 80.0f);
    sf::Text btnSkinClassic("[ Skorka: Classic ]", font, 20); btnSkinClassic.setPosition(20.0f, 150.0f);
    sf::Text btnSkinModern("[ Skorka: Modern ]", font, 20);   btnSkinModern.setPosition(220.0f, 150.0f);
    sf::Text btnSkinGreen("[ Skorka: Green ]", font, 20);       btnSkinGreen.setPosition(420.0f, 150.0f);
    sf::Text btnStart(">>> START GRY <<<", font, 32); btnStart.setPosition(20.0f, 250.0f);

    sf::Text msgEnd("", font, 20); 
    sf::Text txtTimer("", font, 20);
    sf::Text txtMines("", font, 20);

    sf::Text btnOptions("[ Opcje ]", font, 20); 
    btnOptions.setPosition(10.0f, 12.0f);

    sf::Text dropRestart("Restart", font, 18);   dropRestart.setPosition(15.0f, TOP_UI_HEIGHT + 10.0f);
    sf::Text dropSkin("Zmien Skorke", font, 18); dropSkin.setPosition(15.0f, TOP_UI_HEIGHT + 40.0f);
    sf::Text dropFullscreen("Pelny Ekran (F11)", font, 18); dropFullscreen.setPosition(15.0f, TOP_UI_HEIGHT + 70.0f);
    sf::Text dropMenu("Wroc do Menu", font, 18); dropMenu.setPosition(15.0f, TOP_UI_HEIGHT + 100.0f);
    
    // Zwiększone tło, by pomieścić nowy przycisk F11
    sf::RectangleShape dropdownBg(sf::Vector2f(200.0f, 130.0f));
    dropdownBg.setFillColor(sf::Color(50, 50, 50, 240)); 
    dropdownBg.setPosition(10.0f, TOP_UI_HEIGHT);

    // Tło gry (renderowane pod kafelkami, na nim opiera się logika letterboxingu)
    sf::RectangleShape gameBg;

    auto styleButton = [](sf::Text& btn, bool isSelected, sf::Vector2f mousePos) {
        if (isSelected) btn.setFillColor(sf::Color(0, 255, 0));
        else if (btn.getGlobalBounds().contains(mousePos)) btn.setFillColor(sf::Color(255, 255, 0));
        else btn.setFillColor(sf::Color(255, 255, 255));
    };

    while (window.isOpen()) {
        // MAPOWANIE MYSZY: To najważniejsza linijka przy skalowaniu okna!
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

        if (currentState == GameState::Playing && !isFirstClick) {
            elapsedTime = static_cast<int>(gameClock.getElapsedTime().asSeconds());
        }

        if (currentState == GameState::Menu) {
            styleButton(btnEasy, columns == 9, mousePos);
            styleButton(btnMedium, columns == 16, mousePos);
            styleButton(btnHard, columns == 30, mousePos);
            styleButton(btnSkinClassic, activeSkin == "classic", mousePos);
            styleButton(btnSkinModern, activeSkin == "modern", mousePos);
            styleButton(btnSkinGreen, activeSkin == "green", mousePos);
            styleButton(btnStart, false, mousePos);
        } else {
            styleButton(btnOptions, isDropdownOpen, mousePos); 
            styleButton(msgEnd, false, mousePos);
            
            if (isDropdownOpen) {
                styleButton(dropRestart, false, mousePos);
                styleButton(dropSkin, false, mousePos);
                styleButton(dropFullscreen, false, mousePos);
                styleButton(dropMenu, false, mousePos);
            }
        }

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();

            // Zdarzenie zmiany rozmiaru okna
            if (event.type == sf::Event::Resized) {
                adjustView(window);
            }

            // Klawisz F11 obsługiwany globalnie
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11) {
                toggleFullscreen(window);
            }

            if (event.type == sf::Event::MouseButtonPressed) {
                if (event.mouseButton.button == sf::Mouse::Left) {
                    
                    if (currentState == GameState::Menu) {
                        if (btnEasy.getGlobalBounds().contains(mousePos)) { columns = 9; rows = 9; minesCount = 10; }
                        else if (btnMedium.getGlobalBounds().contains(mousePos)) { columns = 16; rows = 16; minesCount = 40; }
                        else if (btnHard.getGlobalBounds().contains(mousePos)) { columns = 30; rows = 16; minesCount = 99; }
                        else if (btnSkinClassic.getGlobalBounds().contains(mousePos)) { activeSkin = "classic"; loadTextures(activeSkin); }
                        else if (btnSkinModern.getGlobalBounds().contains(mousePos)) { activeSkin = "modern"; loadTextures(activeSkin); }
                        else if (btnSkinGreen.getGlobalBounds().contains(mousePos)) { activeSkin = "green"; loadTextures(activeSkin); }
                        else if (btnStart.getGlobalBounds().contains(mousePos)) {
                            unsigned int w = columns * TILE_SIZE; 
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
                                applyWindowSize(window, 650, 350); 
                            }
                            isDropdownOpen = false; 
                        }
                        else if (btnOptions.getGlobalBounds().contains(mousePos)) {
                            isDropdownOpen = true;
                        }
                        else {
                            int x = static_cast<int>(mousePos.x) / TILE_SIZE;
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
                    else if (currentState == GameState::GameOver || currentState == GameState::Victory) {
                        if (isDropdownOpen) {
                            if (dropRestart.getGlobalBounds().contains(mousePos)) initGame();
                            else if (dropSkin.getGlobalBounds().contains(mousePos)) cycleSkin();
                            else if (dropFullscreen.getGlobalBounds().contains(mousePos)) toggleFullscreen(window);
                            else if (dropMenu.getGlobalBounds().contains(mousePos)) {
                                currentState = GameState::Menu;
                                applyWindowSize(window, 650, 350);
                            }
                            isDropdownOpen = false;
                        }
                        else if (btnOptions.getGlobalBounds().contains(mousePos)) {
                            isDropdownOpen = true;
                        }
                        else if (!btnOptions.getGlobalBounds().contains(mousePos)) {
                            initGame();
                        }
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Right && currentState == GameState::Playing && !isDropdownOpen) {
                    int x = static_cast<int>(mousePos.x) / TILE_SIZE;
                    int y = (static_cast<int>(mousePos.y) - TOP_UI_HEIGHT) / TILE_SIZE;
                    if (isValid(x, y) && !grid[getIndex(x, y)].isRevealed) {
                        grid[getIndex(x, y)].isFlagged = !grid[getIndex(x, y)].isFlagged;
                        if (grid[getIndex(x, y)].isFlagged) flagsPlaced++;
                        else flagsPlaced--;
                    }
                }
            }
        }

        // TŁO: Aby czarne paski letterboxingu działały, czyścimy okno na całkowicie czarno
        window.clear(sf::Color::Black);

        // A tu rysujemy właściwe szare tło dopasowane do aktualnej gry
        gameBg.setSize(sf::Vector2f(logicalW, logicalH));
        gameBg.setFillColor(sf::Color(70, 70, 70));
        window.draw(gameBg);

        if (currentState == GameState::Menu) {
            window.draw(titleMenu);
            window.draw(btnEasy); window.draw(btnMedium); window.draw(btnHard);
            window.draw(btnSkinClassic); window.draw(btnSkinModern); window.draw(btnSkinGreen);
            window.draw(btnStart);
        } 
        else {
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
                    sprite.setPosition(static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE + TOP_UI_HEIGHT));
                    window.draw(sprite);
                }
            }

            float currentWidth = columns * TILE_SIZE * 1.0f;
            sf::RectangleShape topBar(sf::Vector2f(currentWidth, TOP_UI_HEIGHT * 1.0f));
            topBar.setFillColor(sf::Color(40, 40, 40));
            window.draw(topBar);
            
            window.draw(btnOptions); 

            txtTimer.setString("Czas: " + std::to_string(elapsedTime) + "s");
            txtTimer.setPosition(currentWidth / 2.0f - 40.0f, 12.0f);
            window.draw(txtTimer);

            txtMines.setString("Miny: " + std::to_string(minesCount - flagsPlaced));
            txtMines.setPosition(currentWidth - 100.0f, 12.0f);
            window.draw(txtMines);

            if (currentState == GameState::GameOver || currentState == GameState::Victory) {
                sf::RectangleShape overlay(sf::Vector2f(currentWidth, 60.0f));
                overlay.setFillColor(sf::Color(0, 0, 0, 200));
                overlay.setPosition(0.0f, TOP_UI_HEIGHT + (rows * TILE_SIZE) / 2.0f - 30.0f);
                window.draw(overlay);

                msgEnd.setString(currentState == GameState::Victory ? "WYGRANA! Kliknij by zagrac" : "PRZEGRANA! Kliknij by zagrac");
                msgEnd.setPosition(currentWidth / 2.0f - msgEnd.getGlobalBounds().width / 2.0f, TOP_UI_HEIGHT + (rows * TILE_SIZE) / 2.0f - 15.0f);
                window.draw(msgEnd);
            }

            if (isDropdownOpen) {
                window.draw(dropdownBg);
                window.draw(dropRestart);
                window.draw(dropSkin);
                window.draw(dropFullscreen);
                window.draw(dropMenu);
            }
        }

        window.display();
    }

    return 0;
}