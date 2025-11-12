#pragma once
#include <SFML/Graphics.hpp>
//#include "snake.h"
#include "food.h"

class Snake; //What this is doing is declaring Snake is a class

// World objects
extern sf::RectangleShape Field;
extern sf::Vector2i appleXY;
extern sf::RectangleShape BigWallN;
extern sf::RectangleShape BigWallE;
extern sf::RectangleShape BigWallS;
extern sf::RectangleShape BigWallW;

// Function to initialize world
void initWorld(float SCREEN_WIDTH, float SCREEN_HEIGHT, float wallthickness);

// Function to draw world
void drawWorld(sf::RenderWindow& window);

// Draws the sanke
void drawSnake(sf::RenderWindow& window, const Snake& snakelist, const int& CELL_SIZE);

// Draws the apple
void drawApple(sf::RenderWindow& window,sf::Vector2i appleXY);