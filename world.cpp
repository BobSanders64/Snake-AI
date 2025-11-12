#include <SFML/Graphics.hpp>
#include "world.h"
#include <iostream>
#include "snake.h"
#include "GridMath.h"

// Define the global shapes declared with extern in the header
sf::RectangleShape Field;
sf::RectangleShape BigWallN;
sf::RectangleShape BigWallE;
sf::RectangleShape BigWallS;
sf::RectangleShape BigWallW;

void initWorld(float SCREEN_WIDTH, float SCREEN_HEIGHT, float wallthickness)
{
    const float EWwallpos = SCREEN_WIDTH-wallthickness;

    // Initialize the existing shapes (not create new ones)
    Field.setFillColor(sf::Color::Green);
    Field.setSize(sf::Vector2f(SCREEN_HEIGHT, SCREEN_HEIGHT));
    Field.setOutlineColor(sf::Color::Red);
    Field.setOutlineThickness(-wallthickness);
    Field.setPosition({0, 0});
    
    BigWallN.setSize(sf::Vector2f(SCREEN_WIDTH, wallthickness));
    BigWallN.setFillColor(sf::Color(153, 157, 160, 200));
    BigWallN.setPosition({0, 0});
    
    BigWallE.setSize(sf::Vector2f(wallthickness, SCREEN_HEIGHT));
    BigWallE.setFillColor(sf::Color(153, 157, 160, 200));
    BigWallE.setPosition({EWwallpos, 0});
    
    BigWallS.setSize(sf::Vector2f(SCREEN_WIDTH, wallthickness));
    BigWallS.setFillColor(sf::Color(153, 157, 160, 200));
    BigWallS.setPosition({0, EWwallpos});
    
    BigWallW.setSize(sf::Vector2f(wallthickness, SCREEN_HEIGHT));
    BigWallW.setFillColor(sf::Color(153, 157, 160, 200));
    BigWallW.setPosition({0, 0});
}

void drawWorld(sf::RenderWindow& window)
{
    window.draw(Field);           
    window.draw(BigWallN); // Draw the walls
    window.draw(BigWallE);
    window.draw(BigWallS);
    window.draw(BigWallW);
}

void drawSnake(sf::RenderWindow& window, const Snake& snake, const int& CELL_SIZE)
{
    
    //Loads everything as textures
    static sf::Texture HeadOfSnake, BodyOfSnake, TailOfSnake, WS, WN, EN, ES;
    static bool textureLoaded = false;
    
    if (!textureLoaded)
    {
        bool loadSuccess = true;
        
        loadSuccess &= HeadOfSnake.loadFromFile("../SnakeHead.png");
        loadSuccess &= BodyOfSnake.loadFromFile("../SnakeBody.png");
        loadSuccess &= TailOfSnake.loadFromFile("../SnakeEnd.png");
        loadSuccess &= EN.loadFromFile("../sEN.png");
        loadSuccess &= ES.loadFromFile("../sES.png");
        loadSuccess &= WN.loadFromFile("../sWN.png");
        loadSuccess &= WS.loadFromFile("../sWS.png");
        
        if (!loadSuccess) {
            std::cout << "'\n'Snake textures could not be loaded\n";
        } else {
            textureLoaded = true;
        }
    }

    sf::Sprite HeadSprite(HeadOfSnake);
    sf::Sprite BodySprite(BodyOfSnake);
    sf::Sprite TailSprite(TailOfSnake);
    sf::Sprite WSS(WS);
    sf::Sprite WNS(WN);
    sf::Sprite ESS(ES);
    sf::Sprite ENS(EN);

    // Set origins for proper rotation (center of each sprite)
    HeadSprite.setOrigin(sf::Vector2f(HeadOfSnake.getSize().x / 2.f, HeadOfSnake.getSize().y / 2.f));
    BodySprite.setOrigin(sf::Vector2f(BodyOfSnake.getSize().x / 2.f, BodyOfSnake.getSize().y / 2.f));
    TailSprite.setOrigin(sf::Vector2f(TailOfSnake.getSize().x / 2.f, TailOfSnake.getSize().y / 2.f));
    WSS.setOrigin(sf::Vector2f(WS.getSize().x / 2.f, WS.getSize().y / 2.f));
    WNS.setOrigin(sf::Vector2f(WN.getSize().x / 2.f, WN.getSize().y / 2.f));
    ESS.setOrigin(sf::Vector2f(ES.getSize().x / 2.f, ES.getSize().y / 2.f));
    ENS.setOrigin(sf::Vector2f(EN.getSize().x / 2.f, EN.getSize().y / 2.f));

    
 // Draw each segment with the appropriate sprite
    for (size_t i = 0; i < snake.body.size(); ++i)
    {
        const auto& segment = snake.body[i];
        sf::Vector2f pos = GridMath::GridToScreenPos(segment.x, segment.y);

        pos.x += CELL_SIZE / 2.0f;
        pos.y += CELL_SIZE / 2.0f;
        
        // Determine which sprite to use based on position in snake
        if (i == snake.body.size()-1)
        {
            // Head (first element)
            HeadSprite.setPosition(pos);
            if (snake.direction.x==-1 && snake.direction.y==0) //left
            {
                HeadSprite.setRotation(sf::degrees(0));
            }
            else if (snake.direction.x==1 && snake.direction.y==0) //right
            {
                HeadSprite.setRotation(sf::degrees(180));
            }
            else if (snake.direction.x==0 && snake.direction.y==-1) //up
            {
                HeadSprite.setRotation(sf::degrees(90));
            }
            else if (snake.direction.x==0 && snake.direction.y==1) //down
            {
                HeadSprite.setRotation(sf::degrees(270));
            }
            window.draw(HeadSprite);
        }
        else if (i == 0) {
            // Tail (last element)
            TailSprite.setPosition(pos);

            if (snake.body.size() > 1) {
                const auto& afterTail = snake.body[i+1];
                
                if (afterTail.x > segment.x) { // Tail points left
                    TailSprite.setRotation(sf::degrees(180));
                }
                else if (afterTail.x < segment.x) { // Tail points right
                    TailSprite.setRotation(sf::degrees(0));
                }
                else if (afterTail.y > segment.y) { // Tail points up
                    TailSprite.setRotation(sf::degrees(-90));
                }
                else { // Tail points down
                    TailSprite.setRotation(sf::degrees(-270));
                }
            }
            
            window.draw(TailSprite);
        }
        else {
            // Body segments - check if corner
            bool isCorner = false;
            
            // Only check for corners if we have previous and next segments
            if (i > 0 && i < snake.body.size() - 1) {
                const auto& prev = snake.body[i-1];
                const auto& next = snake.body[i+1];

                int fromDirX = segment.x - prev.x;  // Direction we came from
                int fromDirY = segment.y - prev.y;
                int toDirX = next.x - segment.x;    // Direction we're going
                int toDirY = next.y - segment.y;
                
                // A corner is when direction changes (X and Y both change)
                isCorner = (fromDirX != toDirX || fromDirY != toDirY);
                
                if (isCorner)
                {
                    // Determine which corner sprite to use
                    sf::Sprite* cornerSprite; // Default

                    // Coming from East (fromDirX < 0) going North (toDirY < 0)
                    if (fromDirX<0 && toDirY<0)
                    {
                        cornerSprite = &ENS;
                    }
                    // Coming from West (fromDirX > 0) going South (toDirY > 0)
                    else if (fromDirX > 0 && toDirY > 0)
                    {
                        cornerSprite = &WSS;
                    }
                    // Coming from East (fromDirX < 0) going South (toDirY > 0)
                    else if (fromDirX < 0 && toDirY > 0)
                    {
                        cornerSprite = &ESS;
                    }
                    // Coming from West (fromDirX > 0) going North (toDirY < 0)
                    else if (fromDirX > 0 && toDirY < 0)
                    {
                        cornerSprite = &WNS;
                    }
                    // Coming from North (fromDirY > 0) going East (toDirX > 0)
                    else if (fromDirY > 0 && toDirX > 0)
                    {
                        cornerSprite = &ENS;
                    }
                    // Coming from South (fromDirY < 0) going East (toDirX > 0)
                    else if (fromDirY < 0 && toDirX > 0)
                    {
                        cornerSprite = &ESS;
                    }
                    // Coming from North (fromDirY > 0) going West (toDirX < 0)
                    else if (fromDirY > 0 && toDirX < 0)
                    {
                        cornerSprite = &WNS;
                    }
                    // Coming from South (fromDirY < 0) going West (toDirX < 0)
                    else if (fromDirY < 0 && toDirX < 0)
                    {
                        cornerSprite = &WSS;
                    }
                    else
                    {
                        cornerSprite = &WSS;
                    }
                    
                    cornerSprite->setPosition(pos);
                    window.draw(*cornerSprite);
                }
            }
            
            if (!isCorner) {
                const auto& prev = snake.body[i-1];

                //Horizontal
                if (prev.x < segment.x && prev.y == segment.y) {
                    BodySprite.setRotation(sf::degrees(0));
                    BodySprite.setPosition(pos);
                }
                else if (prev.x > segment.x && prev.y == segment.y) {
                    BodySprite.setRotation(sf::degrees(180));
                    BodySprite.setPosition(pos);
                }
                //Vertical
                else if (prev.x == segment.x && prev.y < segment.y) {
                    BodySprite.setRotation(sf::degrees(90));
                    BodySprite.setPosition(pos);
                }
                else if (prev.x == segment.x && prev.y > segment.y) {
                    BodySprite.setRotation(sf::degrees(270));
                    BodySprite.setPosition(pos);
                }
                window.draw(BodySprite);
            }
        }
    }
}

void drawApple(sf::RenderWindow& window,sf::Vector2i appleXY)
{
    static sf::Texture appleTexture;
    static bool textureLoaded = false;
    
    // Load the texture only once
    if (!textureLoaded) {
        if (!appleTexture.loadFromFile("../Apple.png")) {
            std::cout << "Apple could not be loaded" << std::endl;
        } else {
            textureLoaded = true;
        }
    }
    
    // Create a sprite from the texture
    sf::Sprite appleSprite(appleTexture);
    
    
    sf::Vector2f pos = GridMath::GridToScreenPos(appleXY.x, appleXY.y);
    appleSprite.setPosition(pos);
    window.draw(appleSprite);
}