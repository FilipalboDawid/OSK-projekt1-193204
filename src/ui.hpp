#pragma once
#include <SFML/Graphics.hpp>

void initUI();
void updateUI(sf::Vector2f mousePos);
void renderUI(sf::RenderWindow& window, float offsetX);