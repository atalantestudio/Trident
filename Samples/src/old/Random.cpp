#include "Random.hpp"

std::mt19937 Random::mt;
std::uniform_int_distribution<std::mt19937::result_type> Random::distribution;

void Random::initialize() {
	mt.seed(std::random_device()());
}