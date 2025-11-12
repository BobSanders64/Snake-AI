#include "GridMath.h"
#include "Snake.h"
#include <SFML/Graphics.hpp>
#include <cmath>

sf::Vector2i GridMath::ScreenToGridPos(float x, float y)
{
    return sf::Vector2i(static_cast<int>((x-wallthickness)/CELL_SIZE),static_cast<int>((y-wallthickness)/CELL_SIZE));
}

sf::Vector2f GridMath::GridToScreenPos(float x, float y)
{
    return sf::Vector2f(std::round(wallthickness+x*CELL_SIZE),std::round(wallthickness+y*CELL_SIZE));
}