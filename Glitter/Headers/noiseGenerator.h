#pragma once
#include "glitter.hpp"
#include "FastNoise.h"

class noiseGenerator
{
public:
	noiseGenerator();
	~noiseGenerator();
	GLuint noiseTexture2D(unsigned int dimension);
	GLuint noiseTexture3D(unsigned int dimension);
	FastNoise* noiseEngine;
private:

};