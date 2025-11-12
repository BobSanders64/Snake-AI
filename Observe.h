#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "snake.h"
#include "world.h"
#include "food.h"
#include "HitCheck.h"
#include "GridMath.h"
#include "AI.h"

class Observe
{
public:
    explicit Observe(const std::vector<double>& bestGenome);

    // Runs one game with the best model, returns final score
    int Run();

private:
    std::vector<double> m_best;
};
