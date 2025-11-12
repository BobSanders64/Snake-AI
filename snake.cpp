#pragma once
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <deque>
#include "snake.h"


Snake::Snake()
{
    body.push_back(sf::Vector2i(0, 0));
    direction = sf::Vector2i(1, 0);
}

void Snake::grow()
{
    length = (length+1);
}

sf::Vector2i Snake::SnakeHead()
{
    return body.back();
}


void Snake::move()  //Updates Deque with new positions
{
    // 1. Figure out new head based on direction
    sf::Vector2i oldHead = SnakeHead();
    sf::Vector2i newHead(oldHead.x + direction.x, oldHead.y + direction.y);
    
    // 2. Push the new head at the back of the body
    body.push_back(newHead);
    
    // 3. If we are not growing, pop off the front
    //    if you are always the same length, do:
    if (body.size() > length) //if how big it is bigger to the size of the deque
    {
        body.pop_front();
    }
}

void Snake::SetDirection(int x, int y)
{
    if (!(direction.x+x==0 && direction.y+y==0))
    {
        direction = sf::Vector2i(x, y);
    }
}

bool Snake::checkSelfCollision()
{
    sf::Vector2i head = SnakeHead();
    for (size_t i = 0; i < body.size() - 1; i++)
    {
        if (body[i].x == head.x && body[i].y == head.y)
        {
            return true;
        }
    }
    return false;
}