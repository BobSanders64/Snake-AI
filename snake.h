#pragma once
#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>
#include <deque>

class Snake
{
public:
    int length = 2;
    std::deque<sf::Vector2i> body; // Store body segments as grid positions
    sf::Vector2i direction;

    // Methods declarations (not implementations)
    Snake(); // Constructor
    void grow();
    void SetDirection(int x, int y);
    sf::Vector2i SnakeHead();
    void move();
    bool checkSelfCollision();
};