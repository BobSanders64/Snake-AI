#include "Evolution.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <ctime>

Evolution::Evolution(int popSize, int numPar, int numOff, double crossRate)
    : populationSize(popSize), numParents(numPar), numOffspring(numOff),
      crossoverRate(crossRate), generation(0)
{
    rng.seed(static_cast<unsigned>(std::time(nullptr)));
    population.resize(populationSize);

    // Initialize all-time best with worst possible fitness
    allTimeBest.fitness = -999999.0;
}

void Evolution::initializePopulation()
{
    std::uniform_real_distribution<double> sigmaDist(0.5, 1.5);  // Initial sigma around 1.0

    for (auto& individual : population) {
        // Initialize genome weights
        for (int i = 0; i < GENOME_SIZE; i++) {
            individual.genome[i] = randomDouble(-1.0, 1.0);
        }
        // Initialize single sigma for entire genome
        individual.sigma = sigmaDist(rng);
        individual.fitness = 0.0;
    }
}

void Evolution::evaluatePopulation()
{
    // This will be called from Main with threading
    // Fitness evaluation happens externally
}

void Evolution::evolve()
{
    // Step 1: Sort population by fitness (descending - best first)
    std::sort(population.begin(), population.end(),
        [](const Individual& a, const Individual& b) {
            return a.fitness > b.fitness;
        });

    // Step 2: Top 155 become parents (μ)
    std::vector<Individual> parents(population.begin(), population.begin() + numParents);

    // Step 3: Generate 995 offspring (λ)
    std::vector<Individual> offspring;
    offspring.reserve(numOffspring);

    for (int i = 0; i < numOffspring; i++) {
        // Select two random parents from the parent pool
        std::pair<int, int> parentIndices = selectTwoParents(numParents);

        // Crossover genomes
        std::vector<double> childGenome = crossover(
            parents[parentIndices.first].genome,
            parents[parentIndices.second].genome
        );

        // Crossover sigma (simple: randomly pick from one parent)
        double childSigma;
        if (randomDouble(0.0, 1.0) < 0.5) {
            childSigma = parents[parentIndices.first].sigma;
        } else {
            childSigma = parents[parentIndices.second].sigma;
        }

        // Self-Adaptive Mutation (mutates both genome AND sigma)
        mutate(childGenome, childSigma);

        Individual child;
        child.genome = childGenome;
        child.sigma = childSigma;
        child.fitness = 0.0;  // Will be evaluated next generation
        offspring.push_back(child);
    }

    // Step 4: Create tournament pool = 995 offspring + 5 elite parents
    std::vector<Individual> tournamentPool;
    tournamentPool.reserve(numOffspring + 5);

    // Add all offspring
    tournamentPool.insert(tournamentPool.end(), offspring.begin(), offspring.end());

    // Add top 5 elite parents (guaranteed survival)
    for (int i = 0; i < 5; i++) {
        tournamentPool.push_back(parents[i]);
    }

    // Step 5: Sort tournament pool by fitness
    std::sort(tournamentPool.begin(), tournamentPool.end(),
        [](const Individual& a, const Individual& b) {
            return a.fitness > b.fitness;
        });

    // Step 6: Best 500 from tournament pool become next generation
    population.clear();
    population.insert(population.end(),
                     tournamentPool.begin(),
                     tournamentPool.begin() + populationSize);

    generation++;
}

Individual Evolution::getBest() const
{
    auto best = std::max_element(population.begin(), population.end(),
        [](const Individual& a, const Individual& b) {
            return a.fitness < b.fitness;
        });
    return *best;
}

void Evolution::updateAllTimeBest()
{
    Individual currentBest = getBest();
    if (currentBest.fitness > allTimeBest.fitness) {
        allTimeBest = currentBest;
        std::cout << "*** NEW ALL-TIME BEST: " << allTimeBest.fitness << " ***" << std::endl;
    }
}

std::pair<int, int> Evolution::selectTwoParents(int numParents)
{
    int parent1 = rng() % numParents;
    int parent2 = rng() % numParents;

    // Make sure they're different
    while (parent2 == parent1) {
        parent2 = rng() % numParents;
    }

    return {parent1, parent2};
}

std::vector<double> Evolution::crossover(const std::vector<double>& parent1, const std::vector<double>& parent2)
{
    std::vector<double> child(GENOME_SIZE);

    // Uniform crossover: 75% chance to take from parent1, else parent2
    for (int i = 0; i < GENOME_SIZE; i++) {
        if (randomDouble(0.0, 1.0) < crossoverRate) {
            child[i] = parent1[i];
        } else {
            child[i] = parent2[i];
        }
    }

    return child;
}

void Evolution::mutate(std::vector<double>& genome, double& sigma)
{
    // Self-Adaptive Mutation with SINGLE sigma for entire genome
    // τ (tau) = learning rate = 1/√n

    double tau = 1.0 / std::sqrt(GENOME_SIZE);

    // Generate global noise
    std::normal_distribution<double> globalNoise(0.0, 1.0);
    double globalN = globalNoise(rng);

    // STEP 1: Mutate the single sigma using self-adaptive formula
    // σ' = σ * exp(τ * N(0,1))
    sigma = sigma * std::exp(tau * globalN);

    // Prevent sigma from getting too small (minimum 0.02 to prevent collapse)
    sigma = std::max(0.02, sigma);

    // Also cap it so it doesn't explode
    sigma = std::min(2.0, sigma);

    // STEP 2: Mutate ALL genome values using the SAME sigma
    // x'_i = x_i + σ' * N(0,1)
    for (int i = 0; i < GENOME_SIZE; i++) {
        std::normal_distribution<double> mutationNoise(0.0, 1.0);
        double noise = mutationNoise(rng);

        genome[i] += sigma * noise;

        // Clamp to reasonable range
        genome[i] = std::max(-5.0, std::min(5.0, genome[i]));
    }
}

double Evolution::randomDouble(double min, double max)
{
    std::uniform_real_distribution<double> dist(min, max);
    return dist(rng);
}

void Evolution::saveBest(const std::string& filename)
{
    Individual best = getBest();
    std::ofstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to save genome to " << filename << std::endl;
        return;
    }

    for (double gene : best.genome) {
        file << gene << "\n";
    }

    file.close();
    std::cout << "Saved best genome (fitness: " << best.fitness << ") to " << filename << std::endl;
}

void Evolution::loadBest(const std::string& filename, std::vector<double>& genome)
{
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Failed to load genome from " << filename << std::endl;
        return;
    }

    genome.clear();
    double gene;
    while (file >> gene) {
        genome.push_back(gene);
    }

    file.close();

    if (genome.size() != GENOME_SIZE) {
        std::cerr << "Warning: Loaded genome size (" << genome.size()
                  << ") doesn't match expected size (" << GENOME_SIZE << ")" << std::endl;
    }

    std::cout << "Loaded genome from " << filename << std::endl;
}