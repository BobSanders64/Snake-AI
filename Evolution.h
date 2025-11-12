#pragma once
#include <vector>
#include <random>
#include "AI.h"

struct Individual
{
    std::vector<double> genome;      // The actual weights (123 genes)
    double sigma;                    // ONE mutation step size for entire genome
    double fitness;

    Individual() : genome(GENOME_SIZE, 0.0), sigma(1.0), fitness(0.0) {}
};

class Evolution
{
public:
    Evolution(int populationSize, int numParents, int numOffspring,
              double crossoverRate);

    // Initialize population with random genomes
    void initializePopulation();

    // Evaluate fitness for all individuals (will be threaded)
    void evaluatePopulation();

    // Create next generation through ES
    void evolve();

    // Get the current population
    std::vector<Individual>& getPopulation() { return population; }

    // Get best individual
    Individual getBest() const;

    // Save/load best genome
    void saveBest(const std::string& filename);
    void loadBest(const std::string& filename, std::vector<double>& genome);

    int getGeneration() const { return generation; }

    // Get all-time best (not just current generation)
    Individual getAllTimeBest() const { return allTimeBest; }

    // Update all-time best if current best is better
    void updateAllTimeBest();

private:
    std::vector<Individual> population;
    Individual allTimeBest;  // Track best solution ever seen
    int populationSize;     // 500
    int numParents;         // μ = 155
    int numOffspring;       // λ = 995
    double crossoverRate;   // 0.75 per gene
    int generation;
    std::mt19937 rng;

    // Select two random parents from the parent pool
    std::pair<int, int> selectTwoParents(int numParents);

    // Crossover: Uniform crossover with crossoverRate per gene
    std::vector<double> crossover(const std::vector<double>& parent1, const std::vector<double>& parent2);

    // Self-Adaptive Mutation: Updates both genome and single sigma
    void mutate(std::vector<double>& genome, double& sigma);

    // Random number generators
    double randomDouble(double min, double max);
};