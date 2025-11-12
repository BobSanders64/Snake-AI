#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cstdlib>
#include <ctime>
#include "snake.h"

extern const int GRID_SIZE;

class Food
{
public:
    int points=0;

    Food();//constructor
    sf::Vector2i apple(const Snake& snakelist);
    int pointz();
};
