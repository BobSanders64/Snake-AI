#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include "world.h"
#include "Snake.h"
#include "food.h"

class Snake;

extern int points;
extern sf::Vector2i appleXY;
extern const int GRID_SIZE;
extern sf::RectangleShape BigWallN;
extern sf::RectangleShape BigWallE;
extern sf::RectangleShape BigWallS;
extern sf::RectangleShape BigWallW;

bool DidItHitWall(const Snake& snakelist);
bool AreYaWinningSon(const Snake& snake);
bool DidItEat(const Snake& snake);