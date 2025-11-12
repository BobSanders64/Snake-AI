/*
#include <SFML/Graphics.hpp>


int main()
{
sf::VideoMode videoMode(sf::Vector2u(400, 300));
sf::RenderWindow window(videoMode, "Basic SFML Test");
    
// Create a green circle
sf::CircleShape shape(50.f); // 50 pixel radius
shape.setFillColor(sf::Color::Green);
shape.setPosition(sf::Vector2f(150.f, 100.f)); // Use Vector2f instead of two floats
    
while (window.isOpen())
{
if (auto event = window.pollEvent())
{
if (event->is<sf::Event::Closed>())
window.close();
}
        
window.clear(sf::Color::Blue); // Clear with blue background
window.draw(shape);           // Draw the circle
window.display();
}
    
return 0;
}
*/