#include <SFML/Graphics.hpp>
#include <algorithm>
#include "globals.hpp"
#include "functions.hpp"
#include "ui.hpp"

int main() {
    sf::RenderWindow window(sf::VideoMode(logicalW, logicalH), "Minesweeper C++", sf::Style::Default);
    window.setFramerateLimit(60);

    if (!font.loadFromFile("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf")) {
        font.loadFromFile("C:/Windows/Fonts/arial.ttf");
    }

    loadTextures(activeSkin);
    loadLeaderboard(); 
    initUI(); // Inicjalizacja wszystkich obiektów tekstowych

    while (window.isOpen()) {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

        float boardWidth = columns * TILE_SIZE;
        float offsetX = (logicalW > boardWidth) ? (logicalW - boardWidth) / 2.0f : 0.0f;

        if (currentState == GameState::Playing && !isFirstClick) {
            elapsedTime = static_cast<int>(gameClock.getElapsedTime().asSeconds());
        }

        updateUI(mousePos); // Podświetlanie przycisków myszką

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::Resized) adjustView(window);
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11) toggleFullscreen(window);

            // Przechwytywanie wpisywania z klawiatury
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

            // Obsługa kliknięć myszką
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

        renderUI(window, offsetX); // Wywołanie całego rysowania z ui.cpp!
        window.display();
    }

    return 0;
}