#pragma once
#include "glitter.hpp"
#include "FastNoise.h"
#include <vector>

class NoiseGenerator : public FastNoise
{
public:
	NoiseGenerator();
	~NoiseGenerator();
	std::vector<std::vector<float>> noiseVector2D(unsigned int dimension, bool invert = false);
	std::vector<std::vector<std::vector<float>>> noiseVector3D(unsigned int dimension, bool invert = false);
	//FastNoise* noiseEngine;
private:

};