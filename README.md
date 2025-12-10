# Snake-AI

Neural network trained via evolutionary strategies to play Snake.

https://github.com/BobSanders64/Snake-AI/raw/main/Snake%20Best%20Sped%20Up.mp4

## Overview

This project uses evolutionary computing to train a neural network to play the classic Snake game. Rather than traditional backpropagation, the network weights are optimized through an evolutionary strategy—populations of neural networks compete, and the best performers pass their genes to the next generation with mutation.

## How It Works

- **Neural Network:** Feedforward network that takes game state as input and outputs movement decisions
- **Evolutionary Strategy:** Tournament-based optimization with selection, crossover, and mutation
- **Fitness Function:** Rewards snake length

## Results

The trained agent learns to efficiently navigate the grid, avoid walls and its own tail, and pursue food. Top performers consistently achieve high scores after several hundred generations of evolution.

## Author

Nathan Honn
