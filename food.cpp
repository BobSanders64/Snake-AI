#include "food.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include "snake.h"

sf::Vector2i appleXY;
bool validposition;

Food::Food()
{
    std::srand(static_cast<unsigned>(std::time(nullptr)));
    appleXY = sf::Vector2i(GRID_SIZE, GRID_SIZE);
}

//    for (const auto& snakebits : snakelist.body) //loop through where the snake is at.
//
sf::Vector2i Food::apple(const Snake& snakelist)
{
    int appleXcord;
    int appleYcord;
    do{
        validposition=true;
        appleXcord=(rand()%(GRID_SIZE-1))+1; //random number from 1 to GRID_SIZE-1
        appleYcord=(rand()%(GRID_SIZE-1))+1; //random number from 1 to GRID_SIZE-1

        for (const auto& snakebits : snakelist.body) //loop through where the snake is at.
        {
            if (snakebits.x==appleXcord && snakebits.y==appleYcord)
            {
                validposition=false;
                break;
            }
        }
    }while(!validposition);
    
    sf::Vector2i appleXY(appleXcord, appleYcord);
    return appleXY;
}

int Food::pointz()
{
    return points=points+1;
}