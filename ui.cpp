#include "ui.hpp"
#include "globals.hpp"
#include "functions.hpp"
#include <algorithm>

void styleButton(sf::Text& btn, bool isSelected, sf::Vector2f mousePos) {
    if (isSelected) btn.setFillColor(sf::Color(0, 255, 0));
    else if (btn.getGlobalBounds().contains(mousePos)) btn.setFillColor(sf::Color(255, 255, 0));
    else btn.setFillColor(sf::Color(255, 255, 255));
}

sf::String getCheckbox(bool opt) { return opt ? L"[X] " : L"[ ] "; }

void initUI() {
    titleMenu = sf::Text(L"USTAWIENIA GRY", font, 30); titleMenu.setPosition(20.0f, 20.0f);
    
    btnBeginner = sf::Text(L"[ Początkujący (8x8, 10 min) ]", font, 20);      btnBeginner.setPosition(20.0f, 70.0f);
    btnIntermediate = sf::Text(L"[ Zaawansowany (16x16, 40 min) ]", font, 20); btnIntermediate.setPosition(20.0f, 100.0f);
    btnExpert = sf::Text(L"[ Ekspert (30x16, 99 min) ]", font, 20);            btnExpert.setPosition(20.0f, 130.0f);
    btnCustom = sf::Text(L"[ Plansza Użytkownika ]", font, 20);                btnCustom.setPosition(20.0f, 160.0f);

    lblCols = sf::Text(L"Szerokość (8-30):", font, 18); lblCols.setPosition(50.0f, 190.0f);
    valCol = sf::Text("10", font, 20);                  valCol.setPosition(250.0f, 188.0f);
    lblRows = sf::Text(L"Wysokość (8-24):", font, 18);  lblRows.setPosition(50.0f, 220.0f);
    valRow = sf::Text("10", font, 20);                  valRow.setPosition(250.0f, 218.0f);
    lblMines = sf::Text(L"Miny (max A*B/3):", font, 18); lblMines.setPosition(50.0f, 250.0f);
    valMine = sf::Text("15", font, 20);                  valMine.setPosition(250.0f, 248.0f);

    btnSkinClassic = sf::Text(L"[ Skórka: Classic ]", font, 20); btnSkinClassic.setPosition(20.0f, 320.0f);
    btnSkinModern = sf::Text(L"[ Skórka: Modern ]", font, 20);   btnSkinModern.setPosition(220.0f, 320.0f);
    btnSkinGreen = sf::Text(L"[ Skórka: Green ]", font, 20);     btnSkinGreen.setPosition(420.0f, 320.0f);
    
    btnStart = sf::Text(L">>> START GRY <<<", font, 32);         btnStart.setPosition(20.0f, 380.0f);
    btnShowLeaderboard = sf::Text(L"[ TABELA WYNIKÓW ]", font, 20); btnShowLeaderboard.setPosition(420.0f, 390.0f);

    float rightColX = 380.0f;
    lblOptionsTitle = sf::Text(L"DODATKOWE USTAWIENIA", font, 22); lblOptionsTitle.setPosition(rightColX, 70.0f);
    lblOptionsTitle.setFillColor(sf::Color::Yellow);
    
    btnOptOpening = sf::Text(getCheckbox(optOpeningMove) + sf::String(L"Ruch otwierający"), font, 18);  btnOptOpening.setPosition(rightColX, 110.0f);
    btnOptQuestion = sf::Text(getCheckbox(optQuestionMarks) + sf::String(L"Pytajniki (PPM)"), font, 18); btnOptQuestion.setPosition(rightColX, 140.0f);
    btnOptChording = sf::Text(getCheckbox(optChording) + sf::String(L"Bezpieczna okolica"), font, 18);   btnOptChording.setPosition(rightColX, 170.0f);
    btnOptRemaining = sf::Text(getCheckbox(optOpenRemaining) + sf::String(L"Otwórz pozostałe"), font, 18); btnOptRemaining.setPosition(rightColX, 200.0f);
    btnOptUndo = sf::Text(getCheckbox(optUndo) + sf::String(L"Rozbrój (Cofanie)"), font, 18);            btnOptUndo.setPosition(rightColX, 230.0f);
    btnOptHints = sf::Text(getCheckbox(optHints) + sf::String(L"Podpowiedź (Kl. H)"), font, 18);         btnOptHints.setPosition(rightColX, 260.0f);

    msgEnd = sf::Text("", font, 20); 
    txtTimer = sf::Text("", font, 20);
    txtMines = sf::Text("", font, 20);
    btnOptions = sf::Text(L"[ Opcje ]", font, 20); btnOptions.setPosition(10.0f, 12.0f);

    dropRestart = sf::Text(L"Restart", font, 18);   dropRestart.setPosition(15.0f, TOP_UI_HEIGHT + 10.0f);
    dropUndo = sf::Text(L"-> Cofnij Ruch", font, 18); dropUndo.setPosition(15.0f, TOP_UI_HEIGHT + 40.0f); dropUndo.setFillColor(sf::Color::Cyan);
    dropSkin = sf::Text(L"Zmień Skórkę", font, 18); dropSkin.setPosition(15.0f, TOP_UI_HEIGHT + 70.0f);
    dropFullscreen = sf::Text(L"Pełny Ekran (F11)", font, 18); dropFullscreen.setPosition(15.0f, TOP_UI_HEIGHT + 100.0f);
    dropMenu = sf::Text(L"Wróć do Menu", font, 18); dropMenu.setPosition(15.0f, TOP_UI_HEIGHT + 130.0f);
    
    dropdownBg.setSize(sf::Vector2f(200.0f, 160.0f));
    dropdownBg.setFillColor(sf::Color(50, 50, 50, 240)); 
    dropdownBg.setPosition(10.0f, TOP_UI_HEIGHT);

    txtTitle = sf::Text("", font, 40); txtTitle.setPosition(50.0f, 50.0f);
    txtSubtitle = sf::Text("", font, 24); txtSubtitle.setPosition(50.0f, 120.0f);
    btnReturnMenu = sf::Text(L"[ Wróć do Menu ]", font, 24); btnReturnMenu.setPosition(50.0f, 480.0f);

    tooltipText = sf::Text("", font, 14);
    tooltipText.setFillColor(sf::Color::White);
    tooltipBg.setFillColor(sf::Color(20, 20, 20, 240)); 
    tooltipBg.setOutlineThickness(1.0f);
    tooltipBg.setOutlineColor(sf::Color::Yellow);
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

        btnOptOpening.setString(getCheckbox(optOpeningMove) + sf::String(L"Ruch otwierający"));
        btnOptQuestion.setString(getCheckbox(optQuestionMarks) + sf::String(L"Pytajniki (PPM)"));
        btnOptChording.setString(getCheckbox(optChording) + sf::String(L"Bezpieczna okolica"));
        btnOptRemaining.setString(getCheckbox(optOpenRemaining) + sf::String(L"Otwórz pozostałe"));
        btnOptUndo.setString(getCheckbox(optUndo) + sf::String(L"Rozbrój (Cofanie)"));
        btnOptHints.setString(getCheckbox(optHints) + sf::String(L"Podpowiedź (Kl. H)"));

        styleButton(btnOptOpening, false, mousePos);
        styleButton(btnOptQuestion, false, mousePos);
        styleButton(btnOptChording, false, mousePos);
        styleButton(btnOptRemaining, false, mousePos);
        styleButton(btnOptUndo, false, mousePos);
        styleButton(btnOptHints, false, mousePos);

        showTooltip = false;
        if (btnOptOpening.getGlobalBounds().contains(mousePos)) {
            tooltipText.setString(L"Pierwszy ruch gwarantuje puste pole\ni otwiera bezpieczny obszar wokół.");
            showTooltip = true;
        } else if (btnOptQuestion.getGlobalBounds().contains(mousePos)) {
            tooltipText.setString(L"Kliknij prawym ponownie na flagę,\nby zmienić ją w znak zapytania (?).");
            showTooltip = true;
        } else if (btnOptChording.getGlobalBounds().contains(mousePos)) {
            tooltipText.setString(L"Kliknij w odkrytą cyfrę, jeśli wokół niej\njest poprawna liczba flag, by otworzyć resztę.");
            showTooltip = true;
        } else if (btnOptRemaining.getGlobalBounds().contains(mousePos)) {
            tooltipText.setString(L"Gdy oflagujesz wszystkie miny, kliknij\nw licznik na górze, by odkryć całą planszę.");
            showTooltip = true;
        } else if (btnOptUndo.getGlobalBounds().contains(mousePos)) {
            tooltipText.setString(L"Zapisuje planszę przed ryzykownym ruchem.\nPo wybuchu możesz cofnąć czas w menu Opcje.");
            showTooltip = true;
        } else if (btnOptHints.getGlobalBounds().contains(mousePos)) {
            tooltipText.setString(L"Najedź na pole i wciśnij 'H', by oflagować\nbombę lub bezpiecznie odkryć puste pole.");
            showTooltip = true;
        }

        if (showTooltip) {
            sf::FloatRect textBounds = tooltipText.getLocalBounds();
            float bgW = textBounds.width + 16.0f;
            float bgH = textBounds.height + 12.0f;
            
            float posX = mousePos.x + 15.0f;
            float posY = mousePos.y + 15.0f;

            if (posX + bgW > logicalW) posX = mousePos.x - bgW - 5.0f;
            if (posY + bgH > logicalH) posY = mousePos.y - bgH - 5.0f;

            tooltipBg.setSize(sf::Vector2f(bgW, bgH));
            tooltipBg.setPosition(posX, posY);
            tooltipText.setPosition(posX + 8.0f, posY + 4.0f);
        }

    } 
    else if (currentState == GameState::Playing || currentState == GameState::GameOver) {
        styleButton(btnOptions, isDropdownOpen, mousePos); 
        styleButton(msgEnd, false, mousePos);
        styleButton(txtMines, false, mousePos); 
        
        if (isDropdownOpen) {
            styleButton(dropRestart, false, mousePos);
            styleButton(dropSkin, false, mousePos);
            styleButton(dropFullscreen, false, mousePos);
            styleButton(dropMenu, false, mousePos);
            if (currentState == GameState::GameOver && optUndo) {
                if (dropUndo.getGlobalBounds().contains(mousePos)) dropUndo.setFillColor(sf::Color::Yellow);
                else dropUndo.setFillColor(sf::Color::Cyan); 
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

        if (showTooltip) {
            window.draw(tooltipBg);
            window.draw(tooltipText);
        }
    } 
    else if (currentState == GameState::Playing || currentState == GameState::GameOver) {
        for (int y = 0; y < rows; ++y) {
            for (int x = 0; x < columns; ++x) {
                Cell cell = grid[getIndex(x, y)];
                const sf::Texture* currentTex = &texHidden;
                
                if (!cell.isRevealed) {
                    if (cell.flagState == 1) currentTex = &texFlag;
                    else if (cell.flagState == 2) currentTex = &texQuestion;
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

        txtTimer.setString(sf::String(L"Czas: ") + std::to_string(elapsedTime) + sf::String(L"s"));
        txtTimer.setPosition(logicalW / 2.0f - 40.0f, 12.0f);
        window.draw(txtTimer);

        txtMines.setString(sf::String(L"Miny: ") + std::to_string(minesCount - flagsPlaced));
        txtMines.setPosition(logicalW - 120.0f, 12.0f);
        window.draw(txtMines);

        // --- POPRAWKA EKRANU KOŃCA GRY ---
        if (currentState == GameState::GameOver) {
            msgEnd.setCharacterSize(18); // Zmniejszamy minimalnie czcionkę
            
            // Przełamanie na dwie linijki za pomocą \n
            if (optUndo) {
                msgEnd.setString(L"PRZEGRANA! Kliknij by zagrać\nlub wejdź w Opcje by cofnąć");
            } else {
                msgEnd.setString(L"PRZEGRANA! Kliknij by zagrać");
            }

            sf::FloatRect bounds = msgEnd.getLocalBounds();
            float overlayHeight = bounds.height + 40.0f; // Dynamiczna wysokość czarnego paska

            sf::RectangleShape overlay(sf::Vector2f(logicalW, overlayHeight));
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            // Centrowanie ciemnego paska w pionie
            overlay.setPosition(0.0f, TOP_UI_HEIGHT + (rows * TILE_SIZE) / 2.0f - overlayHeight / 2.0f);
            window.draw(overlay);

            // Centrowanie tekstu idealnie na środku planszy
            msgEnd.setPosition(logicalW / 2.0f - bounds.width / 2.0f, TOP_UI_HEIGHT + (rows * TILE_SIZE) / 2.0f - bounds.height / 2.0f - 5.0f);
            window.draw(msgEnd);
        }

        if (isDropdownOpen) {
            window.draw(dropdownBg);
            window.draw(dropRestart); 
            if (currentState == GameState::GameOver && optUndo) window.draw(dropUndo);
            window.draw(dropSkin); 
            window.draw(dropFullscreen); window.draw(dropMenu);
        }
    }
    else if (currentState == GameState::EnterName) {
        applyWindowSize(window, 750, 550); 
        txtTitle.setString(L"WYGRANA!");
        txtTitle.setFillColor(sf::Color::Green);
        
        sf::String subText = L"Czas: ";
        subText += std::to_string(elapsedTime);
        subText += L"s\nWpisz swoje imię (i naciśnij ENTER):\n\n> ";
        subText += playerName;
        subText += L"_";
        
        txtSubtitle.setString(subText);
        window.draw(txtTitle);
        window.draw(txtSubtitle);
        window.draw(btnReturnMenu);
    }
    else if (currentState == GameState::Leaderboard) {
        applyWindowSize(window, 750, 550);
        txtTitle.setString(L"TABELA WYNIKÓW");
        txtTitle.setFillColor(sf::Color::White);
        window.draw(txtTitle);

        sf::String diffNames[] = {L"Początkujący", L"Zaawansowany", L"Ekspert", L"Własna Plansza"};
        for (int i = 0; i < 4; ++i) {
            sf::Text header(diffNames[i], font, 20);
            header.setFillColor(sf::Color::Yellow);
            header.setPosition(50.0f, 120.0f + i * 90.0f);
            window.draw(header);

            sf::String scoresText = L"";
            int limit = std::min((int)leaderboards[i].size(), 3); 
            if (limit == 0) scoresText = L"Brak wyników.";
            for (int j = 0; j < limit; ++j) {
                scoresText += std::to_string(j+1) + ". ";
                scoresText += leaderboards[i][j].name;
                scoresText += L" - ";
                scoresText += std::to_string(leaderboards[i][j].time) + "s\n";
            }
            sf::Text scoresList(scoresText, font, 16);
            scoresList.setPosition(50.0f, 150.0f + i * 90.0f);
            window.draw(scoresList);
        }
        window.draw(btnReturnMenu);
    }
}