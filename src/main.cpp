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
    initUI(); 

    while (window.isOpen()) {
        sf::Vector2i pixelPos = sf::Mouse::getPosition(window);
        sf::Vector2f mousePos = window.mapPixelToCoords(pixelPos);

        float boardWidth = columns * TILE_SIZE;
        float offsetX = (logicalW > boardWidth) ? (logicalW - boardWidth) / 2.0f : 0.0f;

        if (currentState == GameState::Playing && !isFirstClick) {
            elapsedTime = static_cast<int>(gameClock.getElapsedTime().asSeconds());
        }

        updateUI(mousePos, offsetX); 

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::Resized) adjustView(window);
            
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::F11) toggleFullscreen(window);
                if (event.key.code == sf::Keyboard::Escape) {
                    if (currentState == GameState::Menu) window.close();
                    else if (currentState == GameState::Playing || currentState == GameState::GameOver) {
                        isDropdownOpen = !isDropdownOpen;
                        isSkinDropdownOpen = false; // Wyzerowanie dla bezpieczeństwa
                    }
                }
                if (event.key.code == sf::Keyboard::H) {
                    if (currentState == GameState::Playing && optHints && !isDropdownOpen) {
                        if (mousePos.x >= offsetX && mousePos.x < offsetX + boardWidth && mousePos.y >= TOP_UI_HEIGHT) {
                            int x = static_cast<int>(mousePos.x - offsetX) / TILE_SIZE;
                            int y = (static_cast<int>(mousePos.y) - TOP_UI_HEIGHT) / TILE_SIZE;
                            if (isValid(x, y) && !grid[getIndex(x, y)].isRevealed && !isFirstClick) { 
                                saveState();
                                if (grid[getIndex(x, y)].isMine) {
                                    if (grid[getIndex(x, y)].flagState != 1) { grid[getIndex(x, y)].flagState = 1; flagsPlaced++; }
                                } else {
                                    revealCell(x, y); checkVictory();
                                }
                            }
                        }
                    }
                }
            }

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
                        
                        if (isSkinDropdownOpen) {
                            if (optSkinClassic.getGlobalBounds().contains(mousePos)) setSkin("classic");
                            else if (optSkinModern.getGlobalBounds().contains(mousePos)) setSkin("modern");
                            else if (optSkinGreen.getGlobalBounds().contains(mousePos)) setSkin("green");
                            isSkinDropdownOpen = false; 
                        }
                        else if (btnSkinSelect.getGlobalBounds().contains(mousePos)) {
                            isSkinDropdownOpen = true;
                        }
                        else {
                            if (btnBeginner.getGlobalBounds().contains(mousePos)) selectedDifficulty = 0;
                            else if (btnIntermediate.getGlobalBounds().contains(mousePos)) selectedDifficulty = 1;
                            else if (btnExpert.getGlobalBounds().contains(mousePos)) selectedDifficulty = 2;
                            else if (btnCustom.getGlobalBounds().contains(mousePos)) selectedDifficulty = 3;
                            
                            if (btnOptOpening.getGlobalBounds().contains(mousePos)) optOpeningMove = !optOpeningMove;
                            if (btnOptQuestion.getGlobalBounds().contains(mousePos)) optQuestionMarks = !optQuestionMarks;
                            if (btnOptChording.getGlobalBounds().contains(mousePos)) optChording = !optChording;
                            if (btnOptRemaining.getGlobalBounds().contains(mousePos)) optOpenRemaining = !optOpenRemaining;
                            if (btnOptUndo.getGlobalBounds().contains(mousePos)) optUndo = !optUndo;
                            if (btnOptHints.getGlobalBounds().contains(mousePos)) optHints = !optHints;

                            if (selectedDifficulty == 3) {
                                if (valCol.getGlobalBounds().contains(mousePos)) activeInputField = 1;
                                else if (valRow.getGlobalBounds().contains(mousePos)) activeInputField = 2;
                                else if (valMine.getGlobalBounds().contains(mousePos)) activeInputField = 3;
                            }

                            if (btnShowLeaderboard.getGlobalBounds().contains(mousePos)) currentState = GameState::Leaderboard;
                            else if (btnQuit.getGlobalBounds().contains(mousePos)) window.close(); 
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
                    }
                    else if (currentState == GameState::Playing || currentState == GameState::GameOver) {
                        if (isDropdownOpen) {
                            bool handled = true;

                            // 1. Zagnieżdżona lista Skórek
                            if (isSkinDropdownOpen && bgSkinDropdown.getGlobalBounds().contains(mousePos)) {
                                if (optSkinClassic.getGlobalBounds().contains(mousePos)) setSkin("classic");
                                else if (optSkinModern.getGlobalBounds().contains(mousePos)) setSkin("modern");
                                else if (optSkinGreen.getGlobalBounds().contains(mousePos)) setSkin("green");
                            }
                            else if (dropRestart.getGlobalBounds().contains(mousePos)) initGame();
                            else if (dropFullscreen.getGlobalBounds().contains(mousePos)) toggleFullscreen(window);
                            else if (currentState == GameState::GameOver && optUndo && dropUndo.getGlobalBounds().contains(mousePos)) undoState();
                            else if (dropMenu.getGlobalBounds().contains(mousePos)) {
                                currentState = GameState::Menu;
                                applyWindowSize(window, 750, 550); 
                            }
                            else if (dropQuit.getGlobalBounds().contains(mousePos)) window.close();
                            else if (dropSkin.getGlobalBounds().contains(mousePos)) {
                                handled = false; // Najechaliśmy na "Zmień Skórkę", więc ignorujemy i NIE zamykamy Opcji
                            }
                            
                            // Zamykanie obu menu po udanej akcji
                            if (handled) {
                                isDropdownOpen = false; 
                                isSkinDropdownOpen = false;
                            }
                        }
                        else if (btnOptions.getGlobalBounds().contains(mousePos)) isDropdownOpen = true;
                        else if (currentState == GameState::Playing && optOpenRemaining && txtMines.getGlobalBounds().contains(mousePos)) {
                            openRemainingSafeCells();
                        }
                        else if (currentState == GameState::Playing || (currentState == GameState::GameOver && !btnOptions.getGlobalBounds().contains(mousePos))) {
                            if (currentState == GameState::GameOver) {
                                initGame();
                            } else {
                                if (mousePos.x >= offsetX && mousePos.x < offsetX + boardWidth && mousePos.y >= TOP_UI_HEIGHT) {
                                    int x = static_cast<int>(mousePos.x - offsetX) / TILE_SIZE;
                                    int y = (static_cast<int>(mousePos.y) - TOP_UI_HEIGHT) / TILE_SIZE;
                                    if (isValid(x, y)) {
                                        if (!grid[getIndex(x, y)].isRevealed && grid[getIndex(x, y)].flagState == 0) {
                                            if (isFirstClick) { generateMines(x, y); isFirstClick = false; gameClock.restart(); }
                                            saveState(); 
                                            if (grid[getIndex(x, y)].isMine) { currentState = GameState::GameOver; revealAllMines(); } 
                                            else { revealCell(x, y); checkVictory(); }
                                        }
                                        else if (grid[getIndex(x, y)].isRevealed && optChording) { chordCell(x, y); }
                                    }
                                }
                            }
                        }
                    }
                    else if (currentState == GameState::EnterName || currentState == GameState::Leaderboard) {
                        if (btnReturnMenu.getGlobalBounds().contains(mousePos)) {
                            currentState = GameState::Menu;
                            applyWindowSize(window, 750, 550);
                        }
                    }
                }
                else if (event.mouseButton.button == sf::Mouse::Right && currentState == GameState::Playing && !isDropdownOpen) {
                    if (mousePos.x >= offsetX && mousePos.x < offsetX + boardWidth && mousePos.y >= TOP_UI_HEIGHT) {
                        int x = static_cast<int>(mousePos.x - offsetX) / TILE_SIZE;
                        int y = (static_cast<int>(mousePos.y) - TOP_UI_HEIGHT) / TILE_SIZE;
                        if (isValid(x, y) && !grid[getIndex(x, y)].isRevealed) {
                            int idx = getIndex(x, y);
                            saveState(); 

                            if (grid[idx].flagState == 0) {
                                grid[idx].flagState = 1; flagsPlaced++;
                            } else if (grid[idx].flagState == 1) {
                                flagsPlaced--; 
                                grid[idx].flagState = optQuestionMarks ? 2 : 0;
                            } else if (grid[idx].flagState == 2) {
                                grid[idx].flagState = 0;
                            }
                        }
                    }
                }
            }
        }

        renderUI(window, offsetX); 
        window.display();
    }

    return 0;
}