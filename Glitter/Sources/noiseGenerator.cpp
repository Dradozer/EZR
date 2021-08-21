#include "NoiseGenerator.h"

NoiseGenerator::NoiseGenerator() {

    this->SetNoiseType(FastNoise::Perlin);
}

NoiseGenerator::~NoiseGenerator() {
	delete this;
}

std::vector<std::vector<float>> NoiseGenerator::noiseVector2D(unsigned int dimension, bool invert) {

	std::vector<std::vector<float>> noiseVector(dimension, std::vector<float>(dimension));

	for (int y = 0; y < dimension; y++) {
		for (int x = 0; x < dimension; x++) {
			noiseVector[x][y] = (invert) ? 1 - this->GetNoise(x, y) : this->GetNoise(x, y);
		}
	}

	return noiseVector;
}

std::vector<std::vector<std::vector<float>>> NoiseGenerator::noiseVector3D(unsigned int dimension, bool invert) {

	std::vector<std::vector<std::vector<float>>> noiseVector(dimension, std::vector<std::vector<float>>(dimension,std:: vector<float>(dimension)));;

	for (int z = 0; z < dimension; z++) {
		for (int y = 0; y < dimension; y++) {
			for (int x = 0; x < dimension; x++) {
				noiseVector[x][y][z] = (invert) ? 1 - this->GetNoise(x, y, z) : this->GetNoise(x, y, z);
			}
		}
	}

	return noiseVector;
}
