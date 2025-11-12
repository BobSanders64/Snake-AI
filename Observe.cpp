#include "Observe.h"
#include "PostGameMenu.h"

// Globals defined in Main.cpp
extern float SCREEN_WIDTH;
extern float SCREEN_HEIGHT;
extern int amt;
extern const int GRID_SIZE;
extern float wallthickness;
extern const float CELL_SIZE;
extern bool gameOver;
extern bool specialmessage;
extern bool ContinueGame;
extern bool AIMODE;

// From world/food
extern sf::Vector2i appleXY;

Observe::Observe(const std::vector<double>& bestGenome)
    : m_best(bestGenome)
{
}

int Observe::Run()
{
    // Reset shared game state for a clean episode
    AIMODE        = true;   // we're letting AI drive
    gameOver      = false;
    ContinueGame  = true;
    specialmessage = false;
    amt           = 0;

    // Create window (SFML 3 style)
    sf::VideoMode videoMode(sf::Vector2u(
        static_cast<unsigned>(SCREEN_WIDTH),
        static_cast<unsigned>(SCREEN_HEIGHT)
    ));
    sf::RenderWindow window(videoMode, "Snake AI - Observe Best");
    window.setFramerateLimit(60);

    // Init world & entities
    initWorld(SCREEN_WIDTH, SCREEN_HEIGHT, wallthickness);

    Snake snake;
    Food apple;

    appleXY = apple.apple(snake); // first apple

    sf::Clock moveClock;
    double moveInterval = 0.13; // same speed as your main loop

    while (window.isOpen() && !gameOver && ContinueGame)
    {
        // --- Events (SFML 3: pollEvent -> optional) ---
        while (auto event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
                ContinueGame = false;
                break;
            }
        }
        if (!window.isOpen() || !ContinueGame)
            break;

        // --- Step the AI & game at fixed interval ---
        if (moveClock.getElapsedTime().asSeconds() >= moveInterval && !gameOver)
        {
            // 1) Build inputs from current state
            // appleXY is already in grid coords
            std::vector<double> inputs =
                AI::getSensorInputs(snake, appleXY, GRID_SIZE);

            // 2) Forward pass with best genome
            std::vector<double> outputs =
                AI::forward(inputs, m_best);

            // 3) Map outputs -> direction (assume sf::Vector2i)
            sf::Vector2i dir = AI::getDirection(outputs, snake);
            snake.SetDirection(dir.x, dir.y);

            // 4) Move snake
            snake.move();

            // 5) Collisions / score logic (mirrors your main loop)
            if (snake.checkSelfCollision())
                gameOver = true;

            if (DidItHitWall(snake))
                gameOver = true;

            if (AreYaWinningSon(snake))
            {
                gameOver = true;
                specialmessage = true;
            }

            if (DidItEat(snake))
            {
                snake.grow();
                amt    = apple.pointz();
                appleXY = apple.apple(snake);
            }

            moveClock.restart();
        }

        // --- Render current state ---
        window.clear();
        drawWorld(window);
        drawApple(window, appleXY);
        drawSnake(window, snake, CELL_SIZE);
        PostGameMenu::ShowPts(window);  // Show score

        window.display();
    }

    // When we leave (death/win/close), return the score this genome achieved
    return amt;
}