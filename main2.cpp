#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <optional>
#include <random>

const int TILE_SIZE = 32;
const int COLUMNS = 15;
const int ROWS = 15;
const int MINES_COUNT = 30;
const int TOP_UI_HEIGHT = 50; // NOWOŚĆ: Miejsce na górne menu

struct Cell {
    bool isMine = false;
    bool isRevealed = false;
    bool isFlagged = false;
    int adjacentMines = 0;
};

enum class GameState { Menu, Playing, GameOver, Victory };

std::vector<Cell> grid(COLUMNS * ROWS);
GameState currentState = GameState::Menu;
std::string activeSkin = "classic"; // Zapamiętuje wybraną skórkę do restartu

sf::Texture texHidden, texEmpty, texMine, texFlag;
sf::Texture texNumbers[8];
sf::Font font;

int getIndex(int x, int y) { return y * COLUMNS + x; }
bool isValid(int x, int y) { return x >= 0 && x < COLUMNS && y >= 0 && y < ROWS; }

bool loadTextures(const std::string& skinName) {
    std::string path = "assets/" + skinName + "/";
    
    if (!texHidden.loadFromFile(path + "hidden.png") ||
        !texEmpty.loadFromFile(path + "empty.png") ||
        !texMine.loadFromFile(path + "mine.png") ||
        !texFlag.loadFromFile(path + "flag.png")) {
        std::cerr << "Blad: Nie znaleziono podstawowych tekstur w " << path << "\n";
        return false;
    }

    for (int i = 0; i < 8; ++i) {
        if (!texNumbers[i].loadFromFile(path + std::to_string(i + 1) + ".png")) {
            return false;
        }
    }
    return true;
}

void initGame() {
    grid.assign(COLUMNS * ROWS, Cell());
    currentState = GameState::Playing;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> disX(0, COLUMNS - 1);
    std::uniform_int_distribution<> disY(0, ROWS - 1);

    int minesPlaced = 0;
    while (minesPlaced < MINES_COUNT) {
        int x = disX(gen);
        int y = disY(gen);
        int idx = getIndex(x, y);

        if (!grid[idx].isMine) {
            grid[idx].isMine = true;
            minesPlaced++;
        }
    }

    for (int y = 0; y < ROWS; ++y) {
        for (int x = 0; x < COLUMNS; ++x) {
            if (grid[getIndex(x, y)].isMine) continue;
            
            int count = 0;
            for (int dy = -1; dy <= 1; ++dy) {
                for (int dx = -1; dx <= 1; ++dx) {
                    if (isValid(x + dx, y + dy) && grid[getIndex(x + dx, y + dy)].isMine) {
                        count++;
                    }
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

int main() {
    // Okno powiększone w pionie o TOP_UI_HEIGHT
    sf::RenderWindow window(sf::VideoMode({COLUMNS * TILE_SIZE, ROWS * TILE_SIZE + TOP_UI_HEIGHT}), "Minesweeper C++");
    window.setFramerateLimit(60);

    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        font.openFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf");
    }

    // --- ELEMENTY INTERFEJSU ---
    sf::Text btnClassic(font, "[ Wybierz skorke: Classic ]", 24);
    btnClassic.setPosition({50.0f, 100.0f});

    sf::Text btnModern(font, "[ Wybierz skorke: Modern ]", 24);
    btnModern.setPosition({50.0f, 160.0f});

    sf::Text btnRestart(font, "[ Restart ]", 20);
    btnRestart.setPosition({10.0f, 12.0f});

    sf::Text btnMenu(font, "[ Wroc do Menu ]", 20);
    btnMenu.setPosition({120.0f, 12.0f});
    
    // sf::Text btnSkin(font, "[ Wybierz skorke ]", 20);
    // btnMenu.setPosition({120.0f, 12.0f});

    sf::Text msgEnd(font, "", 28);
    // Tekst końcowy wyśrodkowany
    msgEnd.setPosition({40.0f, TOP_UI_HEIGHT + (ROWS * TILE_SIZE) / 2.0f - 20.0f});

    while (window.isOpen()) {
        // Pobieranie aktualnej pozycji myszy
        sf::Vector2i mousePosInt = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos({static_cast<float>(mousePosInt.x), static_cast<float>(mousePosInt.y)});

        // --- EFEKTY HOVER (Podświetlanie tekstu po najechaniu) ---
        btnClassic.setFillColor(btnClassic.getGlobalBounds().contains(mousePos) ? sf::Color(255, 255, 0) : sf::Color(255, 255, 255));
        btnModern.setFillColor(btnModern.getGlobalBounds().contains(mousePos) ? sf::Color(255, 255, 0) : sf::Color(255, 255, 255));
        btnRestart.setFillColor(btnRestart.getGlobalBounds().contains(mousePos) ? sf::Color(255, 255, 0) : sf::Color(255, 255, 255));
        btnMenu.setFillColor(btnMenu.getGlobalBounds().contains(mousePos) ? sf::Color(255, 255, 0) : sf::Color(255, 255, 255));
        msgEnd.setFillColor(msgEnd.getGlobalBounds().contains(mousePos) ? sf::Color(255, 255, 0) : sf::Color(255, 255, 255));

        // --- OBSŁUGA ZDARZEŃ ---
        while (const std::optional<sf::Event> event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) window.close();

            if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (mouse->button == sf::Mouse::Button::Left) {
                    
                    if (currentState == GameState::Menu) {
                        // Klikanie w menu głównym
                        if (btnClassic.getGlobalBounds().contains(mousePos)) {
                            activeSkin = "classic";
                            if (loadTextures(activeSkin)) initGame();
                        } else if (btnModern.getGlobalBounds().contains(mousePos)) {
                            activeSkin = "modern";
                            if (loadTextures(activeSkin)) initGame();
                        }
                    }
                    else if (currentState == GameState::Playing) {
                        // Klikanie w górne menu
                        if (btnRestart.getGlobalBounds().contains(mousePos)) {
                            initGame(); // Szybki restart z obecną skórką
                        } else if (btnMenu.getGlobalBounds().contains(mousePos)) {
                            currentState = GameState::Menu;
                        }
                        // else if (btnSkin.getGlobalBounds().contains(mousePos)) {
                        //     currentState = GameState::Menu;
                        // }
                        // Klikanie w planszę
                        else {
                            int x = mousePosInt.x / TILE_SIZE;
                            int y = (mousePosInt.y - TOP_UI_HEIGHT) / TILE_SIZE; // Przesunięcie współrzędnych!
                            int idx = getIndex(x, y);

                            if (isValid(x, y) && !grid[idx].isFlagged) {
                                if (grid[idx].isMine) {
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
                        // Po przegranej/wygranej kliknij w napis lub górne menu by zagrać ponownie
                        if (msgEnd.getGlobalBounds().contains(mousePos) || btnRestart.getGlobalBounds().contains(mousePos)) {
                            initGame();
                        } else if (btnMenu.getGlobalBounds().contains(mousePos)) {
                            currentState = GameState::Menu;
                        }
                    }
                }
                else if (mouse->button == sf::Mouse::Button::Right && currentState == GameState::Playing) {
                    int x = mousePosInt.x / TILE_SIZE;
                    int y = (mousePosInt.y - TOP_UI_HEIGHT) / TILE_SIZE;
                    int idx = getIndex(x, y);

                    if (isValid(x, y) && !grid[idx].isRevealed) {
                        grid[idx].isFlagged = !grid[idx].isFlagged;
                    }
                }
            }
        }

        // --- RENDEROWANIE ---
        window.clear(sf::Color(70, 70, 70)); // Ciemniejsze tło główne

        if (currentState == GameState::Menu) {
            window.draw(btnClassic);
            window.draw(btnModern);
        } 
        else {
            // 1. Rysowanie planszy z przesunięciem o TOP_UI_HEIGHT w dół
            for (int y = 0; y < ROWS; ++y) {
                for (int x = 0; x < COLUMNS; ++x) {
                    Cell cell = grid[getIndex(x, y)];
                    const sf::Texture* currentTex = &texHidden;
                    if (!cell.isRevealed) {
                        if (cell.isFlagged) currentTex = &texFlag;
                    } else {
                        if (cell.isMine) currentTex = &texMine;
                        else if (cell.adjacentMines == 0) currentTex = &texEmpty;
                        else currentTex = &texNumbers[cell.adjacentMines - 1];
                    }

                    sf::Sprite sprite(*currentTex);
                    sprite.setPosition({static_cast<float>(x * TILE_SIZE), static_cast<float>(y * TILE_SIZE + TOP_UI_HEIGHT)});
                    window.draw(sprite);
                }
            }

            // 2. Rysowanie górnego paska (tło + przyciski)
            sf::RectangleShape topBar({COLUMNS * TILE_SIZE * 1.0f, TOP_UI_HEIGHT * 1.0f});
            topBar.setFillColor(sf::Color(40, 40, 40));
            window.draw(topBar);
            window.draw(btnRestart);
            window.draw(btnMenu);

            // 3. Ekran końca gry
            if (currentState == GameState::GameOver || currentState == GameState::Victory) {
                sf::RectangleShape overlay({COLUMNS * TILE_SIZE * 1.0f, 60.0f});
                overlay.setFillColor(sf::Color(0, 0, 0, 200));
                overlay.setPosition({0.0f, TOP_UI_HEIGHT + (ROWS * TILE_SIZE) / 2.0f - 30.0f});
                window.draw(overlay);

                msgEnd.setString(currentState == GameState::Victory ? "WYGRANA! Kliknij by zagrac" : "PRZEGRANA! Kliknij by zagrac");
                window.draw(msgEnd);
            }
        }

        window.display();
    }

    return 0;
}