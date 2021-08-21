#pragma once
#include "glitter.hpp"
#include "NoiseGenerator.h"
#include "Texture.h"
#include "ezr_shader.h"
#include "../Sources/GUI/include/baseObject.h"

struct textureIDs {
	GLuint cloudTexture;
	GLuint detailTexture;
	GLuint motionTexture;
	GLuint weatherTexture;
};

class Clouds : public baseObject
{
public:
	enum Mode { CPU, GPU }; 
	enum TextureClasses {CLOUD,DETAIL,WEATHER};
	
	Clouds(Mode generationMode);
	~Clouds();

    virtual void setGui();
    virtual void draw();
    virtual void update();

    void getIDTexture(TextureClasses generationTexture);
    bool m_runtime_change = false;
	int m_enum;

	GLuint m_cloudTexture;
	GLuint m_detailTexture;
	GLuint m_motionTexture;
	GLuint m_weatherTexture;
	GLuint m_blueNoiseTexture;
private:
	NoiseGenerator* nGenerator;
	Texture* nTexture;
	GLuint m_perlinWorleyCompID;
	GLuint m_worleyCompID;
	GLuint m_weatherCompID;
	GLuint getCloudTexture_CPU();
	GLuint getDetailTexture_CPU();
	GLuint getWeatherTexture_CPU();
	void getCloudTexture_GPU(bool create = false);
	void getDetailTexture_GPU(bool create = false);
	void getWeatherTexture_GPU(bool create = false);
	GLuint getMotionTexture();
	GLuint getBlueNoiseTexture();

    GLuint m_uniform_weather_perlinAmplitude;
    GLuint m_uniform_weather_perlinFrequency;
    GLuint m_uniform_weather_perlinScale;
    GLuint m_uniform_weather_perlinOctaves;

    float m_weather_perlinAmplitude = 0.4;
    float m_weather_perlinFrecuency = 9.7;
    float m_weather_perlinScale = 50.0;
    int m_weather_perlinOctaves = 8;
	float remap(float x, float a, float b, float c, float d);
};