#include "AI.h"
#include <cmath>
#include <algorithm>

// Extract 11 sensor inputs from game state
std::vector<double> AI::getSensorInputs(const Snake& snake, sf::Vector2i foodPos, int gridSize)
{
    std::vector<double> inputs(INPUT_SIZE);
    sf::Vector2i head = snake.body.back();
    sf::Vector2i dir = snake.direction;
    
    // Calculate relative directions based on current heading
    sf::Vector2i leftDir, straightDir, rightDir;
    
    if (dir.x == 1 && dir.y == 0) { // Moving RIGHT
        straightDir = sf::Vector2i(1, 0);
        leftDir = sf::Vector2i(0, -1);     // UP
        rightDir = sf::Vector2i(0, 1);     // DOWN
    }
    else if (dir.x == -1 && dir.y == 0) { // Moving LEFT
        straightDir = sf::Vector2i(-1, 0);
        leftDir = sf::Vector2i(0, 1);      // DOWN
        rightDir = sf::Vector2i(0, -1);    // UP
    }
    else if (dir.x == 0 && dir.y == 1) { // Moving DOWN
        straightDir = sf::Vector2i(0, 1);
        leftDir = sf::Vector2i(1, 0);      // RIGHT
        rightDir = sf::Vector2i(-1, 0);    // LEFT
    }
    else { // Moving UP
        straightDir = sf::Vector2i(0, -1);
        leftDir = sf::Vector2i(-1, 0);     // LEFT
        rightDir = sf::Vector2i(1, 0);     // RIGHT
    }
    
    // Danger sensors (0-2): normalized distance to obstacle
    inputs[0] = checkDanger(snake, leftDir.x, leftDir.y, gridSize);
    inputs[1] = checkDanger(snake, straightDir.x, straightDir.y, gridSize);
    inputs[2] = checkDanger(snake, rightDir.x, rightDir.y, gridSize);
    
    // Food direction (3-6): binary indicators
    inputs[3] = (foodPos.x < head.x) ? 1.0 : 0.0;  // Food left
    inputs[4] = (foodPos.x > head.x) ? 1.0 : 0.0;  // Food right
    inputs[5] = (foodPos.y < head.y) ? 1.0 : 0.0;  // Food up
    inputs[6] = (foodPos.y > head.y) ? 1.0 : 0.0;  // Food down
    
    // Current direction (7-10): one-hot encoding
    inputs[7] = (dir.x == -1) ? 1.0 : 0.0;  // Moving left
    inputs[8] = (dir.x == 1) ? 1.0 : 0.0;   // Moving right
    inputs[9] = (dir.y == -1) ? 1.0 : 0.0;  // Moving up
    inputs[10] = (dir.y == 1) ? 1.0 : 0.0;  // Moving down
    
    return inputs;
}

// Check distance to nearest obstacle in given direction
double AI::checkDanger(const Snake& snake, int dx, int dy, int gridSize)
{
    sf::Vector2i head = snake.body.back();
    int distance = 0;
    
    while (true) {
        distance++;
        int checkX = head.x + (dx * distance);
        int checkY = head.y + (dy * distance);
        
        // Hit wall?
        if (checkX < 0 || checkX >= gridSize || checkY < 0 || checkY >= gridSize) {
            break;
        }
        
        // Hit body?
        if (isBodyAt(snake, checkX, checkY)) {
            break;
        }
    }
    
    // Normalize to 0-1 (higher value = safer, farther from obstacle)
    return distance / static_cast<double>(gridSize);
}

// Check if snake body exists at given position
bool AI::isBodyAt(const Snake& snake, int x, int y)
{
    for (const auto& segment : snake.body) {
        if (segment.x == x && segment.y == y) {
            return true;
        }
    }
    return false;
}

// Neural network forward pass
std::vector<double> AI::forward(const std::vector<double>& inputs, const std::vector<double>& genome)
{
    // Decode genome into weights and biases
    int idx = 0;

    // Input -> Hidden weights [11 x 8 = 88 weights]
    std::vector<std::vector<double>> weightsIH(INPUT_SIZE, std::vector<double>(HIDDEN_SIZE));
    for (int i = 0; i < INPUT_SIZE; i++) {
        for (int h = 0; h < HIDDEN_SIZE; h++) {
            weightsIH[i][h] = genome[idx++];
        }
    }

    // Hidden biases [8]
    std::vector<double> biasH(HIDDEN_SIZE);
    for (int h = 0; h < HIDDEN_SIZE; h++) {
        biasH[h] = genome[idx++];
    }

    // Hidden -> Output weights [8 x 3 = 24 weights]
    std::vector<std::vector<double>> weightsHO(HIDDEN_SIZE, std::vector<double>(OUTPUT_SIZE));
    for (int h = 0; h < HIDDEN_SIZE; h++) {
        for (int o = 0; o < OUTPUT_SIZE; o++) {
            weightsHO[h][o] = genome[idx++];
        }
    }

    // Output biases [3]
    std::vector<double> biasO(OUTPUT_SIZE);
    for (int o = 0; o < OUTPUT_SIZE; o++) {
        biasO[o] = genome[idx++];
    }

    // Forward pass: Input -> Hidden
    std::vector<double> hidden(HIDDEN_SIZE);
    for (int h = 0; h < HIDDEN_SIZE; h++) {
        double sum = biasH[h];
        for (int i = 0; i < INPUT_SIZE; i++) {
            sum += inputs[i] * weightsIH[i][h];
        }
        hidden[h] = tanh_activation(sum);
    }

    // Forward pass: Hidden -> Output
    std::vector<double> outputs(OUTPUT_SIZE);
    for (int o = 0; o < OUTPUT_SIZE; o++) {
        double sum = biasO[o];
        for (int h = 0; h < HIDDEN_SIZE; h++) {
            sum += hidden[h] * weightsHO[h][o];
        }
        outputs[o] = sum;  // No activation on output layer
    }

    return outputs;
}

// Get direction from network output
sf::Vector2i AI::getDirection(const std::vector<double>& outputs, const Snake& snake)
{
    // Find the highest output
    int maxIdx = 0;
    for (int i = 1; i < OUTPUT_SIZE; i++) {
        if (outputs[i] > outputs[maxIdx]) {
            maxIdx = i;
        }
    }

    sf::Vector2i currentDir = snake.direction;
    sf::Vector2i newDir = currentDir;

    // 0 = turn left, 1 = straight, 2 = turn right
    if (maxIdx == 0) { // Turn left
        if (currentDir.x == 1 && currentDir.y == 0) {      // RIGHT -> UP
            newDir = sf::Vector2i(0, -1);
        }
        else if (currentDir.x == -1 && currentDir.y == 0) { // LEFT -> DOWN
            newDir = sf::Vector2i(0, 1);
        }
        else if (currentDir.x == 0 && currentDir.y == 1) {  // DOWN -> RIGHT
            newDir = sf::Vector2i(1, 0);
        }
        else if (currentDir.x == 0 && currentDir.y == -1) { // UP -> LEFT
            newDir = sf::Vector2i(-1, 0);
        }
    }
    else if (maxIdx == 2) { // Turn right
        if (currentDir.x == 1 && currentDir.y == 0) {      // RIGHT -> DOWN
            newDir = sf::Vector2i(0, 1);
        }
        else if (currentDir.x == -1 && currentDir.y == 0) { // LEFT -> UP
            newDir = sf::Vector2i(0, -1);
        }
        else if (currentDir.x == 0 && currentDir.y == 1) {  // DOWN -> LEFT
            newDir = sf::Vector2i(-1, 0);
        }
        else if (currentDir.x == 0 && currentDir.y == -1) { // UP -> RIGHT
            newDir = sf::Vector2i(1, 0);
        }
    }
    // else maxIdx == 1: go straight (keep current direction)

    return newDir;
}

// Activation function
double AI::tanh_activation(double x)
{
    return std::tanh(x);
}