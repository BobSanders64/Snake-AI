#include "PostGameMenu.h"
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <iostream>

enum class GameState
{
    Playing,
    GameOver
};

void PostGameMenu::ShowMenu(sf::RenderWindow& window,bool specialmessage)
{
    GameState CurrentState = GameState::Playing;
    // First, load the font
    static sf::Font gameFont;
    if (!gameFont.openFromFile("../BELL.ttf")) {
        std::cerr << "Failed to load font\n";
    }

    // Then create text objects with the font directly
    float centerpoint=SCREEN_WIDTH/2 - 180;
    sf::Text scoreText(gameFont);
    scoreText.setCharacterSize(24);
    scoreText.setString("Score: "+std::to_string(amt));
    scoreText.setFillColor(sf::Color::Blue);
    scoreText.setPosition({static_cast<float>(centerpoint*1.5), SCREEN_HEIGHT/2 +50});

    sf::Text Author(gameFont);
    Author.setCharacterSize(24);
    Author.setString("Made By: Nathan Jefferson Honn");
    Author.setFillColor(sf::Color::Blue);
    Author.setPosition({static_cast<float>(centerpoint*1.25), SCREEN_HEIGHT/2 +90});

    sf::Text playAgainText(gameFont);

    if (specialmessage==false)
    {
        playAgainText.setString("Game Over! Press Y to Play Again or N to Quit");
        playAgainText.setCharacterSize(24);
        playAgainText.setFillColor(sf::Color::Blue);
        playAgainText.setPosition({centerpoint*0.85f, SCREEN_HEIGHT/2 + 10});
    }
    else if (specialmessage==true)
    {
        playAgainText.setString("YOU WON!!! Press Y to Play Again or N to Quit");
        playAgainText.setCharacterSize(24);
        playAgainText.setFillColor(sf::Color::Yellow);
        playAgainText.setPosition({centerpoint*0.85f, SCREEN_HEIGHT/2 + 10});
    }

    window.draw(scoreText);
    window.draw(playAgainText);
    window.draw(Author);
}

void PostGameMenu::ShowPts(sf::RenderWindow& window)
{
    static sf::Font gameFont;
    if (!gameFont.openFromFile("../BELL.ttf")) {
        std::cerr << "Failed to load font\n";
    }
    
    static sf::Text ptstxt(gameFont);
    static bool initialized = false;
    if (!initialized)
    {
        ptstxt.setCharacterSize(24);
        ptstxt.setFillColor(sf::Color(255, 255, 255, 255));
        ptstxt.setPosition({10.f, 10.f});
        ptstxt.setFillColor(sf::Color::Blue);
        initialized = true;
    }

    // Update the string
    ptstxt.setString("Points: " + std::to_string(amt));

    // Draw
    window.draw(ptstxt);
}