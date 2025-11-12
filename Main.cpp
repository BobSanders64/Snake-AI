#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include "AI.h"
#include "Evolution.h"
#include "snake.h"
#include "food.h"
#include "HitCheck.h"
#include "Observe.h"


float SCREEN_WIDTH = 625.0f;
float SCREEN_HEIGHT = 625.0f;
int amt = 0;

extern const int GRID_SIZE = 25;
float wallthickness = GRID_SIZE / 2.0f;
extern const float CELL_SIZE = (SCREEN_WIDTH - 2.0f * wallthickness) / GRID_SIZE;

bool gameOver = false;
bool specialmessage = false;
bool ContinueGame = true;
bool AIMODE = true;


// Simulation parameters
const int MAX_STEPS = 10000;
const int THREADS = 10;

// Fitness evaluation function (runs headless)
double evaluateFitness(const std::vector<double>& genome, int gridSize)
{
    Snake snake;
    Food food;
    sf::Vector2i appleXY = food.apple(snake);

    int steps = 0;
    int stepsSinceLastFood = 0;
    int foodEaten = 0;
    bool gameOver = false;

    while (!gameOver && steps < MAX_STEPS) {
        // Get sensor inputs
        std::vector<double> inputs = AI::getSensorInputs(snake, appleXY, gridSize);

        // Run neural network
        std::vector<double> outputs = AI::forward(inputs, genome);

        // Get direction decision
        sf::Vector2i newDir = AI::getDirection(outputs, snake);
        snake.SetDirection(newDir.x, newDir.y);

        // Move snake
        snake.move();
        steps++;
        stepsSinceLastFood++;

        // Check wall collision
        if (DidItHitWall(snake)) {
            gameOver = true;
            break;
        }

        // Check self collision
        if (snake.checkSelfCollision()) {
            gameOver = true;
            break;
        }

        // Check if ate food
        sf::Vector2i head = snake.body.back();
        if (head.x == appleXY.x && head.y == appleXY.y) {
            snake.grow();
            foodEaten++;
            stepsSinceLastFood = 0;
            appleXY = food.apple(snake);
        }

        // Timeout if stuck wandering
        if (stepsSinceLastFood > 100) {
            gameOver = true;
            break;
        }
    }

    // Fitness function: heavily reward food, reward survival, penalize wandering
    double fitness = (foodEaten * 1000.0); // + (steps * 1.0) - (stepsSinceLastFood * 2.0)

    return fitness;
}

// Thread worker function
void evaluateIndividuals(std::vector<Individual>& individuals, int start, int end, int gridSize, std::mutex& mtx)
{
    for (int i = start; i < end; i++) {
        double fitness = evaluateFitness(individuals[i].genome, gridSize);

        // Thread-safe update
        std::lock_guard<std::mutex> lock(mtx);
        individuals[i].fitness = fitness;
    }
}

int main()
{
    // Menu system
    std::cout << "=== Snake AI ===" << std::endl;
    std::cout << "Press 0 to TRAIN" << std::endl;
    std::cout << "Press 1 to OBSERVE" << std::endl;
    std::cout << "Your choice: ";

    int choice;
    std::cin >> choice;
    std::cout << std::endl;

    if (choice == 1) {
        // OBSERVE MODE - Display best trained snake
        std::cout << "=== Observation Mode ===" << std::endl;
        std::cout << "Loading best genome from file..." << std::endl;

        // Load the best genome from file
        Evolution evolution(1, 1, 1, 0.0);  // Dummy evolution object just for loading
        evolution.initializePopulation();

        std::vector<double> bestGenome;
        evolution.loadBest("Best Snake Final.txt", bestGenome);

        if (bestGenome.empty()) {
            std::cerr << "Error: Could not load 'Best Snake Final.txt'" << std::endl;
            std::cerr << "Please run training first to generate a saved genome." << std::endl;
            return 1;
        }

        std::cout << "Genome loaded successfully!" << std::endl;
        std::cout << "Starting observation..." << std::endl;

        // Run visual observation with the best genome
        Observe observer(bestGenome);
        observer.Run();

        return 0;
    }
    else if (choice == 0) {
        // TRAINING MODE
        const int POPULATION_SIZE = 500;
        const int NUM_PARENTS = 155;      // μ
        const int NUM_OFFSPRING = 995;    // λ
        const int GENERATIONS = 1000;
        const double CROSSOVER_RATE = 0.75;
        const int GRID_SIZE = 25;

        std::cout << "=== Training Mode ===" << std::endl;
        std::cout << "Population: " << POPULATION_SIZE << std::endl;
        std::cout << "Parents (μ): " << NUM_PARENTS << std::endl;
        std::cout << "Offspring (λ): " << NUM_OFFSPRING << std::endl;
        std::cout << "Threads: " << THREADS << std::endl;
        std::cout << "Genome size: " << GENOME_SIZE << " genes" << std::endl;
        std::cout << "Architecture: " << INPUT_SIZE << " -> " << HIDDEN_SIZE << " -> " << OUTPUT_SIZE << std::endl;
        std::cout << "Crossover rate: " << CROSSOVER_RATE << " per gene" << std::endl;
        std::cout << "Mutation: SELF-ADAPTIVE (one sigma per genome)" << std::endl;
        std::cout << "\nStarting evolution...\n" << std::endl;

        Evolution evolution(POPULATION_SIZE, NUM_PARENTS, NUM_OFFSPRING, CROSSOVER_RATE);
        evolution.initializePopulation();

        std::mutex mtx;

        for (int gen = 0; gen < GENERATIONS; gen++) {
            // Get population
            std::vector<Individual>& population = evolution.getPopulation();

            // Evaluate fitness using multiple threads
            std::vector<std::thread> threads;
            int individualsPerThread = POPULATION_SIZE / THREADS;

            for (int t = 0; t < THREADS; t++) {
                int start = t * individualsPerThread;
                int end = (t == THREADS - 1) ? POPULATION_SIZE : (t + 1) * individualsPerThread;

                threads.emplace_back(evaluateIndividuals, std::ref(population), start, end, GRID_SIZE, std::ref(mtx));
            }

            // Wait for all threads to complete
            for (auto& thread : threads) {
                thread.join();
            }

            // Update all-time best tracker
            evolution.updateAllTimeBest();

            // Get statistics
            Individual best = evolution.getBest();
            double avgFitness = 0.0;
            double avgSigma = 0.0;
            for (const auto& ind : population) {
                avgFitness += ind.fitness;
                avgSigma += ind.sigma;
            }
            avgFitness /= POPULATION_SIZE;
            avgSigma /= POPULATION_SIZE;

            // Print progress
            std::cout << "Generation " << gen + 1
                      << " | Best: " << best.fitness
                      << " | Avg: " << avgFitness<<'\n';

            // Check for population collapse and inject diversity if needed
            if (best.fitness < 0) {
                std::cout << "*** WARNING: Population collapse detected! Injecting diversity... ***" << std::endl;

                // Keep top 50 individuals, randomize the rest
                std::sort(population.begin(), population.end(),
                    [](const Individual& a, const Individual& b) {
                        return a.fitness > b.fitness;
                    });

                // Reinitialize bottom 450 individuals
                for (int i = 50; i < POPULATION_SIZE; i++) {
                    for (int j = 0; j < 123; j++) {
                        population[i].genome[j] = (rand() / (double)RAND_MAX) * 2.0 - 1.0;
                    }
                    population[i].sigma = 0.5 + (rand() / (double)RAND_MAX);  // 0.5-1.5
                    population[i].fitness = 0.0;
                }
            }

            // Evolve to next generation
            evolution.evolve();
        }

        // Save ALL-TIME best (not just final generation)
        std::cout << "\n=== Saving All-Time Best ===" << std::endl;
        Individual allTimeBest = evolution.getAllTimeBest();
        std::cout << "All-time best fitness: " << allTimeBest.fitness << std::endl;

        // Save the all-time best
        std::ofstream bestFile("Best Snake Final.txt");
        for (double gene : allTimeBest.genome) {
            bestFile << gene << "\n";
        }
        bestFile.close();

        std::cout << "\nTraining complete!" << std::endl;
        std::cout << "All-time best genome saved to Best Snake Final.txt" << std::endl;

        // After evolution is done, run a visual episode with the ALL-TIME best genome
        std::cout << "\nPress Enter to observe the all-time best snake...";
        std::cin.ignore();
        std::cin.get();

        Observe observer(allTimeBest.genome);
        observer.Run();

        return 0;
    }
    else {
        std::cout << "Invalid choice. Please restart and select 0 or 1." << std::endl;
        return 1;
    }
}