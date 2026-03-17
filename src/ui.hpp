#pragma once
#include <SFML/Graphics.hpp>

void initUI();
void updateUI(sf::Vector2f mousePos, float offsetX); // DODANO offsetX
void renderUI(sf::RenderWindow& window, float offsetX);