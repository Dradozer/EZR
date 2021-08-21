#include "noiseGenerator.h"

noiseGenerator::noiseGenerator() {
	noiseEngine = new FastNoise();
	noiseEngine->SetNoiseType(FastNoise::Perlin);
}

noiseGenerator::~noiseGenerator() {
	delete noiseEngine;
}

GLuint noiseGenerator::noiseTexture2D(unsigned int dimension) {

	GLuint textureID;
	float value; 

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dimension, dimension, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	for (int y = 0; y < dimension; y++) {
		for (int x = 0; x < dimension; x++) {
			value = noiseEngine->GetNoise(x, y);
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RED, GL_FLOAT, &value);
		}
	}
	glBindTexture(GL_TEXTURE_2D, 0);
	return textureID;
}

GLuint noiseGenerator::noiseTexture3D(unsigned int dimension) {

	GLuint textureID;
	float value;

	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_3D, textureID);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, dimension, dimension, dimension, 0, GL_RED, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	for (int z = 0; z < dimension; z++) {
		for (int y = 0; y < dimension; y++) {
			for (int x = 0; x < dimension; x++) {
				value = noiseEngine->GetNoise(x, y, z);
				glTexSubImage3D(GL_TEXTURE_3D, 0, x, y, z, 1, 1, 1, GL_RED, GL_FLOAT, &value);
			}
		}
	}
	glBindTexture(GL_TEXTURE_3D, 0);
	return textureID;
}