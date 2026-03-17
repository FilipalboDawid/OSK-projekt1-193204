#include "ui.hpp"
#include "globals.hpp"
#include "functions.hpp"
#include <algorithm>

void styleButton(sf::Text& btn, bool isSelected, sf::Vector2f mousePos) {
    if (isSelected) btn.setFillColor(sf::Color(0, 255, 0));
    else if (btn.getGlobalBounds().contains(mousePos)) btn.setFillColor(sf::Color(255, 255, 0));
    else btn.setFillColor(sf::Color(255, 255, 255));
}

std::string getCheckbox(bool opt) { return opt ? "[X] " : "[ ] "; }

void initUI() {
    titleMenu = sf::Text("USTAWIENIA GRY", font, 30); titleMenu.setPosition(20.0f, 20.0f);
    
    // Klasyczne opcje (lewa kolumna)
    btnBeginner = sf::Text("[ Poczatkujacy (8x8, 10 min) ]", font, 20);      btnBeginner.setPosition(20.0f, 70.0f);
    btnIntermediate = sf::Text("[ Zaawansowany (16x16, 40 min) ]", font, 20); btnIntermediate.setPosition(20.0f, 100.0f);
    btnExpert = sf::Text("[ Ekspert (30x16, 99 min) ]", font, 20);            btnExpert.setPosition(20.0f, 130.0f);
    btnCustom = sf::Text("[ Plansza Uzytkownika ]", font, 20);                btnCustom.setPosition(20.0f, 160.0f);

    lblCols = sf::Text("Szerokosc (8-30):", font, 18); lblCols.setPosition(50.0f, 190.0f);
    valCol = sf::Text("10", font, 20);                 valCol.setPosition(250.0f, 188.0f);
    lblRows = sf::Text("Wysokosc (8-24):", font, 18);  lblRows.setPosition(50.0f, 220.0f);
    valRow = sf::Text("10", font, 20);                 valRow.setPosition(250.0f, 218.0f);
    lblMines = sf::Text("Miny (max A*B/3):", font, 18); lblMines.setPosition(50.0f, 250.0f);
    valMine = sf::Text("15", font, 20);                 valMine.setPosition(250.0f, 248.0f);

    btnSkinClassic = sf::Text("[ Skorka: Classic ]", font, 20); btnSkinClassic.setPosition(20.0f, 320.0f);
    btnSkinModern = sf::Text("[ Skorka: Modern ]", font, 20);   btnSkinModern.setPosition(220.0f, 320.0f);
    btnSkinGreen = sf::Text("[ Skorka: Green ]", font, 20);     btnSkinGreen.setPosition(420.0f, 320.0f);
    
    btnStart = sf::Text(">>> START GRY <<<", font, 32);         btnStart.setPosition(20.0f, 380.0f);
    btnShowLeaderboard = sf::Text("[ TABELA WYNIKOW ]", font, 20); btnShowLeaderboard.setPosition(420.0f, 390.0f);

    // --- NOWE OPCJE (Prawa kolumna) ---
    float rightColX = 380.0f;
    lblOptionsTitle = sf::Text("DODATKOWE USTAWIENIA", font, 22); lblOptionsTitle.setPosition(rightColX, 70.0f);
    lblOptionsTitle.setFillColor(sf::Color::Yellow);
    
    btnOptOpening = sf::Text(getCheckbox(optOpeningMove) + "Ruch otwierajacy", font, 18);  btnOptOpening.setPosition(rightColX, 110.0f);
    btnOptQuestion = sf::Text(getCheckbox(optQuestionMarks) + "Pytajniki (PPM)", font, 18); btnOptQuestion.setPosition(rightColX, 140.0f);
    btnOptChording = sf::Text(getCheckbox(optChording) + "Bezpieczna okolica", font, 18);   btnOptChording.setPosition(rightColX, 170.0f);
    btnOptRemaining = sf::Text(getCheckbox(optOpenRemaining) + "Otworz pozostale", font, 18); btnOptRemaining.setPosition(rightColX, 200.0f);
    btnOptUndo = sf::Text(getCheckbox(optUndo) + "Rozbroj (Cofanie)", font, 18);            btnOptUndo.setPosition(rightColX, 230.0f);
    btnOptHints = sf::Text(getCheckbox(optHints) + "Podpowiedz (Kl. H)", font, 18);         btnOptHints.setPosition(rightColX, 260.0f);

    // --- ELEMENTY W GRZE ---
    msgEnd = sf::Text("", font, 20); 
    txtTimer = sf::Text("", font, 20);
    txtMines = sf::Text("", font, 20);
    btnOptions = sf::Text("[ Opcje ]", font, 20); btnOptions.setPosition(10.0f, 12.0f);

    dropRestart = sf::Text("Restart", font, 18);   dropRestart.setPosition(15.0f, TOP_UI_HEIGHT + 10.0f);
    dropUndo = sf::Text("-> Cofnij Ruch", font, 18); dropUndo.setPosition(15.0f, TOP_UI_HEIGHT + 40.0f); dropUndo.setFillColor(sf::Color::Cyan);
    dropSkin = sf::Text("Zmien Skorke", font, 18); dropSkin.setPosition(15.0f, TOP_UI_HEIGHT + 70.0f);
    dropFullscreen = sf::Text("Pelny Ekran (F11)", font, 18); dropFullscreen.setPosition(15.0f, TOP_UI_HEIGHT + 100.0f);
    dropMenu = sf::Text("Wroc do Menu", font, 18); dropMenu.setPosition(15.0f, TOP_UI_HEIGHT + 130.0f);
    
    dropdownBg.setSize(sf::Vector2f(200.0f, 160.0f));
    dropdownBg.setFillColor(sf::Color(50, 50, 50, 240)); 
    dropdownBg.setPosition(10.0f, TOP_UI_HEIGHT);

    txtTitle = sf::Text("", font, 40); txtTitle.setPosition(50.0f, 50.0f);
    txtSubtitle = sf::Text("", font, 24); txtSubtitle.setPosition(50.0f, 120.0f);
    btnReturnMenu = sf::Text("[ Wroc do Menu ]", font, 24); btnReturnMenu.setPosition(50.0f, 480.0f);
}

void updateUI(sf::Vector2f mousePos) {
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

        // Aktualizacja tekstów checkboxów
        btnOptOpening.setString(getCheckbox(optOpeningMove) + "Ruch otwierajacy");
        btnOptQuestion.setString(getCheckbox(optQuestionMarks) + "Pytajniki (PPM)");
        btnOptChording.setString(getCheckbox(optChording) + "Bezpieczna okolica");
        btnOptRemaining.setString(getCheckbox(optOpenRemaining) + "Otworz pozostale");
        btnOptUndo.setString(getCheckbox(optUndo) + "Rozbroj (Cofanie)");
        btnOptHints.setString(getCheckbox(optHints) + "Podpowiedz (Kl. H)");

        styleButton(btnOptOpening, false, mousePos);
        styleButton(btnOptQuestion, false, mousePos);
        styleButton(btnOptChording, false, mousePos);
        styleButton(btnOptRemaining, false, mousePos);
        styleButton(btnOptUndo, false, mousePos);
        styleButton(btnOptHints, false, mousePos);
    } 
    else if (currentState == GameState::Playing || currentState == GameState::GameOver) {
        styleButton(btnOptions, isDropdownOpen, mousePos); 
        styleButton(msgEnd, false, mousePos);
        styleButton(txtMines, false, mousePos); // Licznik jako przycisk (do Otwórz Pozostałe)
        
        if (isDropdownOpen) {
            styleButton(dropRestart, false, mousePos);
            styleButton(dropSkin, false, mousePos);
            styleButton(dropFullscreen, false, mousePos);
            styleButton(dropMenu, false, mousePos);
            if (currentState == GameState::GameOver && optUndo) {
                if (dropUndo.getGlobalBounds().contains(mousePos)) dropUndo.setFillColor(sf::Color::Yellow);
                else dropUndo.setFillColor(sf::Color::Cyan); // Specjalny kolor
            }
        }
    }
    else if (currentState == GameState::EnterName || currentState == GameState::Leaderboard) {
        styleButton(btnReturnMenu, false, mousePos);
    }
}

void renderUI(sf::RenderWindow& window, float offsetX) {
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

        window.draw(lblOptionsTitle);
        window.draw(btnOptOpening); window.draw(btnOptQuestion); window.draw(btnOptChording);
        window.draw(btnOptRemaining); window.draw(btnOptUndo); window.draw(btnOptHints);

        window.draw(btnSkinClassic); window.draw(btnSkinModern); window.draw(btnSkinGreen);
        window.draw(btnStart);
        window.draw(btnShowLeaderboard);
    } 
    else if (currentState == GameState::Playing || currentState == GameState::GameOver) {
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < columns; ++x) {
                Cell cell = grid[getIndex(x, y)];
                const sf::Texture* currentTex = &texHidden;
                
                if (!cell.isRevealed) {
                    if (cell.flagState == 1) currentTex = &texFlag;
                    else if (cell.flagState == 2) currentTex = &texQuestion; // NOWOŚĆ: Rysowanie pytajnika
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

            msgEnd.setString("PRZEGRANA! Kliknij by zagrac" + std::string(optUndo ? " lub wejdz w Opcje by cofnac" : ""));
            msgEnd.setPosition(logicalW / 2.0f - msgEnd.getGlobalBounds().width / 2.0f, TOP_UI_HEIGHT + (rows * TILE_SIZE) / 2.0f - 15.0f);
            window.draw(msgEnd);
        }

        if (isDropdownOpen) {
            window.draw(dropdownBg);
            window.draw(dropRestart); 
            // Cofnij rysujemy tylko, jeśli jest włączone i gra jest przegrana
            if (currentState == GameState::GameOver && optUndo) window.draw(dropUndo);
            window.draw(dropSkin); 
            window.draw(dropFullscreen); window.draw(dropMenu);
        }
    }
    else if (currentState == GameState::EnterName) {
        applyWindowSize(window, 750, 550); 
        txtTitle.setString("WYGRANA!");
        txtTitle.setFillColor(sf::Color::Green);
        txtSubtitle.setString("Czas: " + std::to_string(elapsedTime) + "s\nWpisz swoje imie (i nacisnij ENTER):\n\n> " + playerName + "_");
        window.draw(txtTitle);
        window.draw(txtSubtitle);
        window.draw(btnReturnMenu);
    }
    else if (currentState == GameState::Leaderboard) {
        applyWindowSize(window, 750, 550);
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
}