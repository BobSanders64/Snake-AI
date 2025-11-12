#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include "HitCheck.h"
#include "snake.h"
#include "food.h"
#include <cmath>

bool DidItHitWall(const Snake& snake)
{
    sf::Vector2i head = snake.body.back();

    // If head is outside the valid grid [0..(GRID_SIZE-1)],
    // then it must have hit a wall
    if (head.x < 0 || head.x >= GRID_SIZE || head.y < 0 || head.y >= GRID_SIZE)
    {
        return true;
    }
    return false;
}

bool AreYaWinningSon(const Snake& snake)
{
    if (snake.length>=pow(GRID_SIZE,2)-4)
    {
        return true;
    }
    return false;
}

bool DidItEat(const Snake& snake)
{
    sf::Vector2i head = snake.body.back();
    if (head.x==appleXY.x && head.y==appleXY.y)
    {
        return true;
    }
    return false;
}