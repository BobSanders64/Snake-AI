#pragma once
#include <vector>
#include <SFML/System.hpp>
#include "snake.h"

// Neural network architecture
const int INPUT_SIZE = 11;
const int HIDDEN_SIZE = 8;
const int OUTPUT_SIZE = 3;
const int GENOME_SIZE = (INPUT_SIZE * HIDDEN_SIZE) + HIDDEN_SIZE + (HIDDEN_SIZE * OUTPUT_SIZE) + OUTPUT_SIZE;

class AI
{
public:
    // Extract sensor inputs from game state
    static std::vector<double> getSensorInputs(const Snake& snake, sf::Vector2i foodPos, int gridSize);
    
    // Neural network forward pass
    static std::vector<double> forward(const std::vector<double>& inputs, const std::vector<double>& genome);
    
    // Get direction decision from network output
    static sf::Vector2i getDirection(const std::vector<double>& outputs, const Snake& snake);
    
private:
    // Helper: Check distance to obstacle in a direction
    static double checkDanger(const Snake& snake, int dx, int dy, int gridSize);
    
    // Helper: Check if body segment exists at position
    static bool isBodyAt(const Snake& snake, int x, int y);
    
    // Activation function
    static double tanh_activation(double x);
};
