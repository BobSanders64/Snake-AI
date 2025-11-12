#pragma once
#include <SFML/Graphics.hpp>

extern float wallthickness;
extern const float CELL_SIZE;

class GridMath
{
public:
    static sf::Vector2i ScreenToGridPos(float x, float y);
    static sf::Vector2f GridToScreenPos(float x, float y);    
};