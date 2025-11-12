#pragma once
#include <SFML/Graphics.hpp>

extern int amt;
extern float SCREEN_WIDTH;
extern float SCREEN_HEIGHT;
extern bool specialmessage;

class PostGameMenu
{
public:
    static void ShowMenu(sf::RenderWindow& window,bool specialmessage);
    static void ShowPts(sf::RenderWindow& window);
};
