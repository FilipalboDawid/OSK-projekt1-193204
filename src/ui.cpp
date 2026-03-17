#include "ui.hpp"
#include "globals.hpp"
#include "functions.hpp"
#include <algorithm>

// --- NOWY SYSTEM RYSOWANIA 8-BITOWYCH PRZYCISKÓW ---
void drawRetroButton(sf::RenderWindow& window, sf::Text& text, sf::Vector2f mousePos, bool isSelected = false, int style = 0) {
    sf::FloatRect bounds = text.getGlobalBounds();
    bool isHovered = bounds.contains(mousePos);

    sf::RectangleShape bg(sf::Vector2f(bounds.width, bounds.height + 14.0f));
    bg.setPosition(bounds.left, bounds.top - 4.0f);

    sf::Color bgColor(70, 70, 70);
    sf::Color outColor(40, 40, 40);
    sf::Color txtColor = sf::Color::White;

    if (style == 1 || isSelected) {
        bgColor = sf::Color(40, 140, 40);
        outColor = sf::Color(20, 90, 20);
    } else if (style == 2) {
        bgColor = sf::Color(150, 50, 50);
        outColor = sf::Color(90, 20, 20);
    }

    if (isHovered) {
        bgColor = bgColor + sf::Color(40, 40, 40); 
        outColor = sf::Color::White;
        txtColor = sf::Color::Yellow;
    }

    bg.setFillColor(bgColor);
    bg.setOutlineThickness(2.0f);
    bg.setOutlineColor(outColor);

    sf::RectangleShape shadow = bg;
    shadow.setFillColor(sf::Color(0, 0, 0, 180));
    shadow.setOutlineThickness(0);
    shadow.move(4.0f, 4.0f);

    if (!isHovered) window.draw(shadow);

    if (isHovered) {
        bg.move(2.0f, 2.0f);
        text.move(2.0f, 2.0f);
    }

    window.draw(bg);
    text.setFillColor(txtColor);
    window.draw(text);

    if (isHovered) text.move(-2.0f, -2.0f);
}

void drawRetroInput(sf::RenderWindow& window, sf::Text& text, bool isActive) {
    sf::FloatRect bounds = text.getGlobalBounds();
    sf::RectangleShape bg(sf::Vector2f(60.0f, bounds.height + 14.0f));
    bg.setPosition(bounds.left - 5.0f, bounds.top - 4.0f);
    bg.setFillColor(sf::Color(20, 20, 20));
    bg.setOutlineThickness(2.0f);
    bg.setOutlineColor(isActive ? sf::Color::Red : sf::Color(100, 100, 100));

    window.draw(bg);
    text.setFillColor(sf::Color::White);
    window.draw(text);
}

void drawRetroPanel(sf::RenderWindow& window, sf::RectangleShape& bg) {
    bg.setOutlineThickness(4.0f);
    bg.setOutlineColor(sf::Color(100, 100, 100));
    sf::RectangleShape shadow = bg;
    shadow.setFillColor(sf::Color(0, 0, 0, 180));
    shadow.setOutlineThickness(0);
    shadow.move(6.0f, 6.0f);
    window.draw(shadow);
    window.draw(bg);
}

sf::String getCheckbox(bool opt) { return opt ? L"  [X] " : L"  [ ] "; }

void initUI() {
    titleMenu = sf::Text(L"USTAWIENIA GRY", font, 36); titleMenu.setPosition(20.0f, 20.0f);
    titleMenu.setFillColor(sf::Color::Yellow);
    
    // LEWA KOLUMNA (Trudność + główne przyciski)
    btnBeginner = sf::Text(L"  Początkujący (8x8)  ", font, 20);      btnBeginner.setPosition(20.0f, 80.0f);
    btnIntermediate = sf::Text(L"  Zaawansowany (16x16)  ", font, 20); btnIntermediate.setPosition(20.0f, 120.0f);
    btnExpert = sf::Text(L"  Ekspert (30x16)  ", font, 20);            btnExpert.setPosition(20.0f, 160.0f);
    btnCustom = sf::Text(L"  Plansza Użytkownika  ", font, 20);        btnCustom.setPosition(20.0f, 200.0f);

    lblCols = sf::Text(L"Szerokość (8-30):", font, 18); lblCols.setPosition(50.0f, 240.0f);
    valCol = sf::Text("10", font, 20);                  valCol.setPosition(250.0f, 238.0f);
    lblRows = sf::Text(L"Wysokość (8-24):", font, 18);  lblRows.setPosition(50.0f, 270.0f);
    valRow = sf::Text("10", font, 20);                  valRow.setPosition(250.0f, 268.0f);
    lblMines = sf::Text(L"Miny (max A*B/3):", font, 18); lblMines.setPosition(50.0f, 300.0f);
    valMine = sf::Text("15", font, 20);                  valMine.setPosition(250.0f, 298.0f);

    // Przeniesione na dół po lewej, by było wygodniej
    btnStart = sf::Text(L"  >>> START GRY <<<  ", font, 26);    btnStart.setPosition(20.0f, 360.0f);
    btnShowLeaderboard = sf::Text(L"  Tabela Wyników  ", font, 20); btnShowLeaderboard.setPosition(20.0f, 420.0f);
    btnQuit = sf::Text(L"  Wyjdź do Pulpitu  ", font, 20);      btnQuit.setPosition(20.0f, 480.0f);

    // PRAWA KOLUMNA (Opcje + Skórki)
    float rightColX = 380.0f;
    lblOptionsTitle = sf::Text(L"DODATKOWE USTAWIENIA", font, 22); lblOptionsTitle.setPosition(rightColX, 80.0f);
    lblOptionsTitle.setFillColor(sf::Color::Yellow);
    
    btnOptOpening = sf::Text(getCheckbox(optOpeningMove) + sf::String(L"Ruch otwierający  "), font, 18);  btnOptOpening.setPosition(rightColX, 120.0f);
    btnOptQuestion = sf::Text(getCheckbox(optQuestionMarks) + sf::String(L"Pytajniki (PPM)  "), font, 18); btnOptQuestion.setPosition(rightColX, 160.0f);
    btnOptChording = sf::Text(getCheckbox(optChording) + sf::String(L"Bezpieczna okolica  "), font, 18);   btnOptChording.setPosition(rightColX, 200.0f);
    btnOptRemaining = sf::Text(getCheckbox(optOpenRemaining) + sf::String(L"Otwórz pozostałe  "), font, 18); btnOptRemaining.setPosition(rightColX, 240.0f);
    btnOptUndo = sf::Text(getCheckbox(optUndo) + sf::String(L"Rozbrój (Cofanie)  "), font, 18);            btnOptUndo.setPosition(rightColX, 280.0f);
    btnOptHints = sf::Text(getCheckbox(optHints) + sf::String(L"Podpowiedź (Kl. H)  "), font, 18);         btnOptHints.setPosition(rightColX, 320.0f);

    // Lista skórek idealnie pod opcjami z prawej strony
    btnSkinSelect = sf::Text(L"  Skórka: Classic V  ", font, 20); btnSkinSelect.setPosition(rightColX, 370.0f);
    
    msgEnd = sf::Text("", font, 20); 
    txtTimer = sf::Text("", font, 20);
    txtMines = sf::Text("", font, 20);
    
    // UI w trakcie gry
    btnOptions = sf::Text(L"  Opcje  ", font, 20); btnOptions.setPosition(10.0f, 12.0f);

    dropRestart = sf::Text(L"  Restart  ", font, 18);   
    dropUndo = sf::Text(L"  -> Cofnij Ruch  ", font, 18);
    dropSkin = sf::Text(L"  Zmień Skórkę >  ", font, 18); 
    dropFullscreen = sf::Text(L"  Pełny Ekran (F11)  ", font, 18); 
    dropMenu = sf::Text(L"  Wróć do Menu  ", font, 18); 
    dropQuit = sf::Text(L"  Wyjdź z Gry  ", font, 18); 
    
    dropdownBg.setFillColor(sf::Color(40, 40, 40)); 
    dropdownBg.setPosition(10.0f, TOP_UI_HEIGHT + 5.0f);

    optSkinClassic = sf::Text(L"  Classic  ", font, 18);
    optSkinModern = sf::Text(L"  Modern  ", font, 18);
    optSkinGreen = sf::Text(L"  Green  ", font, 18);
    bgSkinDropdown.setFillColor(sf::Color(40, 40, 40));

    txtTitle = sf::Text("", font, 40); txtTitle.setPosition(50.0f, 50.0f);
    txtSubtitle = sf::Text("", font, 24); txtSubtitle.setPosition(50.0f, 120.0f);
    btnReturnMenu = sf::Text(L"  Wróć do Menu  ", font, 24); btnReturnMenu.setPosition(50.0f, 480.0f);

    tooltipText = sf::Text("", font, 14);
    tooltipText.setFillColor(sf::Color::White);
    tooltipBg.setFillColor(sf::Color(20, 20, 20, 240)); 
    tooltipBg.setOutlineThickness(1.0f);
    tooltipBg.setOutlineColor(sf::Color::Yellow);
}

void updateUI(sf::Vector2f mousePos, float offsetX) {
    sf::String skinName = L"Classic";
    if (activeSkin == "modern") skinName = L"Modern";
    else if (activeSkin == "green") skinName = L"Green";
    btnSkinSelect.setString(L"  Skórka: " + skinName + L" V  ");

    if (currentState == GameState::Menu) {
        // Pozycjonowanie listy w Menu Głównym na sztywno obok
        btnSkinSelect.setPosition(380.0f, 370.0f);
        bgSkinDropdown.setPosition(380.0f, 405.0f);
        bgSkinDropdown.setSize(sf::Vector2f(200.0f, 110.0f));
        optSkinClassic.setPosition(385.0f, 415.0f);
        optSkinModern.setPosition(385.0f, 445.0f);
        optSkinGreen.setPosition(385.0f, 475.0f);

        valCol.setString(strCustomCols + (activeInputField == 1 ? "_" : ""));
        valRow.setString(strCustomRows + (activeInputField == 2 ? "_" : ""));
        valMine.setString(strCustomMines + (activeInputField == 3 ? "_" : ""));

        btnOptOpening.setString(getCheckbox(optOpeningMove) + sf::String(L"Ruch otwierający  "));
        btnOptQuestion.setString(getCheckbox(optQuestionMarks) + sf::String(L"Pytajniki (PPM)  "));
        btnOptChording.setString(getCheckbox(optChording) + sf::String(L"Bezpieczna okolica  "));
        btnOptRemaining.setString(getCheckbox(optOpenRemaining) + sf::String(L"Otwórz pozostałe  "));
        btnOptUndo.setString(getCheckbox(optUndo) + sf::String(L"Rozbrój (Cofanie)  "));
        btnOptHints.setString(getCheckbox(optHints) + sf::String(L"Podpowiedź (Kl. H)  "));

        showTooltip = false;
        if (!isSkinDropdownOpen) { 
            if (btnOptOpening.getGlobalBounds().contains(mousePos)) {
                tooltipText.setString(L"Pierwszy ruch gwarantuje puste pole\ni otwiera bezpieczny obszar wokół."); showTooltip = true;
            } else if (btnOptQuestion.getGlobalBounds().contains(mousePos)) {
                tooltipText.setString(L"Kliknij prawym ponownie na flagę,\nby zmienić ją w znak zapytania (?)."); showTooltip = true;
            } else if (btnOptChording.getGlobalBounds().contains(mousePos)) {
                tooltipText.setString(L"Kliknij w odkrytą cyfrę, jeśli wokół niej\njest poprawna liczba flag, by otworzyć resztę."); showTooltip = true;
            } else if (btnOptRemaining.getGlobalBounds().contains(mousePos)) {
                tooltipText.setString(L"Gdy oflagujesz wszystkie miny, kliknij\nw licznik na górze, by odkryć całą planszę."); showTooltip = true;
            } else if (btnOptUndo.getGlobalBounds().contains(mousePos)) {
                tooltipText.setString(L"Zapisuje planszę przed ryzykownym ruchem.\nPo wybuchu możesz cofnąć czas w menu Opcje."); showTooltip = true;
            } else if (btnOptHints.getGlobalBounds().contains(mousePos)) {
                tooltipText.setString(L"Najedź na pole i wciśnij 'H', by oflagować\nbombę lub bezpiecznie odkryć puste pole."); showTooltip = true;
            }
        }

        if (showTooltip) {
            sf::FloatRect textBounds = tooltipText.getLocalBounds();
            float bgW = textBounds.width + 16.0f;
            float bgH = textBounds.height + 12.0f;
            float posX = mousePos.x + 15.0f; float posY = mousePos.y + 15.0f;

            if (posX + bgW > logicalW) posX = mousePos.x - bgW - 5.0f;
            if (posY + bgH > logicalH) posY = mousePos.y - bgH - 5.0f;

            tooltipBg.setSize(sf::Vector2f(bgW, bgH)); tooltipBg.setPosition(posX, posY);
            tooltipText.setPosition(posX + 8.0f, posY + 4.0f);
        }

    } 
    else if (currentState == GameState::Playing || currentState == GameState::GameOver) {
        
        dropRestart.setPosition(15.0f, TOP_UI_HEIGHT + 15.0f);
        int optYOffset = 55;
        if (currentState == GameState::GameOver && optUndo) {
            dropUndo.setPosition(15.0f, TOP_UI_HEIGHT + optYOffset); optYOffset += 40;
        }
        
        dropSkin.setPosition(15.0f, TOP_UI_HEIGHT + optYOffset); 
        float skinY = TOP_UI_HEIGHT + optYOffset; 
        optYOffset += 40;
        
        dropFullscreen.setPosition(15.0f, TOP_UI_HEIGHT + optYOffset); optYOffset += 40;
        dropMenu.setPosition(15.0f, TOP_UI_HEIGHT + optYOffset); optYOffset += 40;
        dropQuit.setPosition(15.0f, TOP_UI_HEIGHT + optYOffset); optYOffset += 40;
        
        dropdownBg.setSize(sf::Vector2f(220.0f, optYOffset + 10.0f));

        bgSkinDropdown.setPosition(240.0f, skinY - 10.0f); 
        bgSkinDropdown.setSize(sf::Vector2f(160.0f, 110.0f));
        optSkinClassic.setPosition(245.0f, skinY);
        optSkinModern.setPosition(245.0f, skinY + 30.0f);
        optSkinGreen.setPosition(245.0f, skinY + 60.0f);

        if (isDropdownOpen) {
            if (dropSkin.getGlobalBounds().contains(mousePos) || bgSkinDropdown.getGlobalBounds().contains(mousePos)) {
                isSkinDropdownOpen = true;
            } else {
                isSkinDropdownOpen = false;
            }
        } else {
            isSkinDropdownOpen = false;
        }
    }
}

void renderUI(sf::RenderWindow& window, float offsetX, sf::Vector2f mousePos) {
    window.clear(sf::Color(25, 25, 35)); 
    gameBg.setSize(sf::Vector2f(logicalW, logicalH));
    gameBg.setFillColor(sf::Color(60, 60, 60));
    window.draw(gameBg);

    if (currentState == GameState::Menu) {
        window.draw(titleMenu);
        drawRetroButton(window, btnBeginner, mousePos, selectedDifficulty == 0);
        drawRetroButton(window, btnIntermediate, mousePos, selectedDifficulty == 1);
        drawRetroButton(window, btnExpert, mousePos, selectedDifficulty == 2);
        drawRetroButton(window, btnCustom, mousePos, selectedDifficulty == 3);
        
        if (selectedDifficulty == 3) {
            window.draw(lblCols); drawRetroInput(window, valCol, activeInputField == 1);
            window.draw(lblRows); drawRetroInput(window, valRow, activeInputField == 2);
            window.draw(lblMines); drawRetroInput(window, valMine, activeInputField == 3);
        }

        window.draw(lblOptionsTitle);
        drawRetroButton(window, btnOptOpening, mousePos, optOpeningMove);
        drawRetroButton(window, btnOptQuestion, mousePos, optQuestionMarks);
        drawRetroButton(window, btnOptChording, mousePos, optChording);
        drawRetroButton(window, btnOptRemaining, mousePos, optOpenRemaining);
        drawRetroButton(window, btnOptUndo, mousePos, optUndo);
        drawRetroButton(window, btnOptHints, mousePos, optHints);

        drawRetroButton(window, btnStart, mousePos, false, 1); 
        drawRetroButton(window, btnQuit, mousePos, false, 2);  
        drawRetroButton(window, btnShowLeaderboard, mousePos, false);
        drawRetroButton(window, btnSkinSelect, mousePos, isSkinDropdownOpen); 

        if (showTooltip) { window.draw(tooltipBg); window.draw(tooltipText); }

        if (isSkinDropdownOpen) {
            drawRetroPanel(window, bgSkinDropdown);
            drawRetroButton(window, optSkinClassic, mousePos, activeSkin == "classic");
            drawRetroButton(window, optSkinModern, mousePos, activeSkin == "modern");
            drawRetroButton(window, optSkinGreen, mousePos, activeSkin == "green");
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
        
        drawRetroButton(window, btnOptions, mousePos, isDropdownOpen);

        txtTimer.setString(sf::String(L"Czas: ") + std::to_string(elapsedTime) + sf::String(L"s"));
        txtTimer.setPosition(logicalW / 2.0f - 40.0f, 12.0f);
        window.draw(txtTimer);

        txtMines.setString(sf::String(L"Miny: ") + std::to_string(minesCount - flagsPlaced));
        txtMines.setPosition(logicalW - 120.0f, 12.0f);
        if (optOpenRemaining && currentState == GameState::Playing) drawRetroButton(window, txtMines, mousePos);
        else window.draw(txtMines);

        if (currentState == GameState::GameOver) {
            msgEnd.setCharacterSize(18); 
            if (optUndo) msgEnd.setString(L"PRZEGRANA! Kliknij by zagrać\nlub wejdź w Opcje by cofnąć");
            else msgEnd.setString(L"PRZEGRANA! Kliknij by zagrać");

            sf::FloatRect bounds = msgEnd.getLocalBounds();
            float overlayHeight = bounds.height + 40.0f; 

            sf::RectangleShape overlay(sf::Vector2f(logicalW, overlayHeight));
            overlay.setFillColor(sf::Color(0, 0, 0, 200));
            overlay.setPosition(0.0f, TOP_UI_HEIGHT + (rows * TILE_SIZE) / 2.0f - overlayHeight / 2.0f);
            window.draw(overlay);

            msgEnd.setPosition(logicalW / 2.0f - bounds.width / 2.0f, TOP_UI_HEIGHT + (rows * TILE_SIZE) / 2.0f - bounds.height / 2.0f - 5.0f);
            window.draw(msgEnd);
        }

        if (isDropdownOpen) {
            drawRetroPanel(window, dropdownBg);
            drawRetroButton(window, dropRestart, mousePos);
            if (currentState == GameState::GameOver && optUndo) {
                drawRetroButton(window, dropUndo, mousePos, false, 2); 
            }
            drawRetroButton(window, dropSkin, mousePos, isSkinDropdownOpen);
            drawRetroButton(window, dropFullscreen, mousePos);
            drawRetroButton(window, dropMenu, mousePos);
            drawRetroButton(window, dropQuit, mousePos, false, 2);
        }

        if (isSkinDropdownOpen && isDropdownOpen) {
            drawRetroPanel(window, bgSkinDropdown);
            drawRetroButton(window, optSkinClassic, mousePos, activeSkin == "classic");
            drawRetroButton(window, optSkinModern, mousePos, activeSkin == "modern");
            drawRetroButton(window, optSkinGreen, mousePos, activeSkin == "green");
        }
    }
    else if (currentState == GameState::EnterName) {
        applyWindowSize(window, 750, 550); 
        txtTitle.setString(L"WYGRANA!"); txtTitle.setFillColor(sf::Color::Green);
        sf::String subText = L"Czas: "; subText += std::to_string(elapsedTime);
        subText += L"s\nWpisz swoje imię (i naciśnij ENTER):\n\n> "; subText += playerName; subText += L"_";
        txtSubtitle.setString(subText);
        window.draw(txtTitle); window.draw(txtSubtitle); 
        drawRetroButton(window, btnReturnMenu, mousePos);
    }
    else if (currentState == GameState::Leaderboard) {
        applyWindowSize(window, 750, 550);
        txtTitle.setString(L"TABELA WYNIKÓW"); txtTitle.setFillColor(sf::Color::White); window.draw(txtTitle);

        sf::String diffNames[] = {L"Początkujący", L"Zaawansowany", L"Ekspert", L"Własna Plansza"};
        for (int i = 0; i < 4; ++i) {
            sf::Text header(diffNames[i], font, 20); header.setFillColor(sf::Color::Yellow); header.setPosition(50.0f, 120.0f + i * 90.0f); window.draw(header);
            sf::String scoresText = L""; int limit = std::min((int)leaderboards[i].size(), 3); 
            if (limit == 0) scoresText = L"Brak wyników.";
            for (int j = 0; j < limit; ++j) {
                scoresText += std::to_string(j+1) + ". "; scoresText += leaderboards[i][j].name; scoresText += L" - "; scoresText += std::to_string(leaderboards[i][j].time) + "s\n";
            }
            sf::Text scoresList(scoresText, font, 16); scoresList.setPosition(50.0f, 150.0f + i * 90.0f); window.draw(scoresList);
        }
        drawRetroButton(window, btnReturnMenu, mousePos);
    }
}