#include "Cloud.h"
#include <iostream>
#include <imgui.h>

Clouds::Clouds(Mode generationMode) {
    nGenerator = new NoiseGenerator();
    nTexture = new Texture();
	m_enum = 2;

	// attach and link shaders once
	ezr::Shader::attachShader(m_perlinWorleyCompID, GL_COMPUTE_SHADER, "perlinworley.glsl");
	ezr::Shader::linkShader(m_perlinWorleyCompID);
	ezr::Shader::attachShader(m_worleyCompID, GL_COMPUTE_SHADER, "worley.glsl");
	ezr::Shader::linkShader(m_worleyCompID);
	ezr::Shader::attachShader(m_weatherCompID, GL_COMPUTE_SHADER, "weather.glsl");
	ezr::Shader::linkShader(m_weatherCompID);

    m_uniform_weather_perlinAmplitude = glGetUniformLocation(m_weatherCompID, "perlinAmplitude");
    m_uniform_weather_perlinFrequency = glGetUniformLocation(m_weatherCompID, "perlinFrecuency");
    m_uniform_weather_perlinScale = glGetUniformLocation(m_weatherCompID, "perlinScale");
    m_uniform_weather_perlinOctaves = glGetUniformLocation(m_weatherCompID, "perlinOctaves");

	switch (generationMode)
	{
	case CPU:
		m_cloudTexture = getCloudTexture_CPU();
		m_detailTexture = getDetailTexture_CPU();
		m_weatherTexture = getWeatherTexture_CPU();
	case GPU:
		getCloudTexture_GPU(true);
		getDetailTexture_GPU(true);
		getWeatherTexture_GPU(true);
	}
	//m_motionTexture = getMotionTexture();
	m_blueNoiseTexture = getBlueNoiseTexture();
}

Clouds::~Clouds() {

}
void Clouds::setGui() {
    ImGui::Begin("Cloud Textures");

    ImGui::SliderInt("Enum", &m_enum, 0, 2);
    switch (m_enum)
    {
        case 0:

        case 1:

        case 2:
            ImGui::TextColored(ImVec4(1, 0, 0, 1), "Weather");
            bool b_amplitude = ImGui::SliderFloat("Amplitude", &m_weather_perlinAmplitude, 0, 10.f);
            bool b_freq = ImGui::SliderFloat("Frequency", &m_weather_perlinFrecuency, 0, 10.f);
            bool b_scale = ImGui::SliderFloat("Scale", &m_weather_perlinScale, 0, 512.f);
            bool b_octav = ImGui::SliderInt("Octaves", &m_weather_perlinOctaves, 0, 128);
            m_runtime_change ^= b_amplitude ^ b_freq ^ b_scale ^ b_octav;
    }




    ImGui::End();
}

void Clouds::draw() {

}

void Clouds::update() {

}

void Clouds::getIDTexture(TextureClasses generationTexture) {
    switch (generationTexture)
    {
        case CLOUD:
            getCloudTexture_GPU();
        case DETAIL:
            getDetailTexture_GPU();
        case WEATHER:
            getWeatherTexture_GPU();
    }
}

GLuint Clouds::getCloudTexture_CPU() {

	unsigned int dimension = 128;
    const auto& range_dim = static_cast<int>(dimension);

	nGenerator->SetNoiseType(FastNoise::PerlinFractal);
	nGenerator->SetFractalType(FastNoise::FBM);
	nGenerator->SetFractalOctaves(5);
	nGenerator->SetFrequency(0.02);
	const auto& pNoise = nGenerator->noiseVector3D(dimension);

	nGenerator->SetNoiseType(FastNoise::Cellular);
	nGenerator->SetCellularReturnType(FastNoise::Distance);
	nGenerator->SetFrequency(0.05);
	const auto& wNoise_1 = nGenerator->noiseVector3D(dimension, true);
	nGenerator->SetFrequency(0.1);
	const auto& wNoise_2 = nGenerator->noiseVector3D(dimension, true);
	nGenerator->SetFrequency(0.2);
	const auto& wNoise_4 = nGenerator->noiseVector3D(dimension, true);
	nGenerator->SetFrequency(0.4);
	const auto& wNoise_8 = nGenerator->noiseVector3D(dimension, true);
	nGenerator->SetFrequency(0.8);
	const auto& wNoise_16 = nGenerator->noiseVector3D(dimension, true);

	const auto& cloudID = nTexture->generate3DTexture(dimension, dimension, dimension, GL_RGBA32F, GL_RGBA, GL_FLOAT);

	float values[4];
	for (int z = 0; z < range_dim; z++) {
		for (int y = 0; y < range_dim; y++) {
			for (int x = 0; x < range_dim; x++) {
				values[1] = wNoise_1[x][y][z] * 0.625 + wNoise_2[x][y][z] * 0.25 + wNoise_4[x][y][z] * 0.125;
				values[2] = wNoise_2[x][y][z] * 0.625 + wNoise_4[x][y][z] * 0.25 + wNoise_8[x][y][z] * 0.125;
				values[3] = wNoise_4[x][y][z] * 0.625 + wNoise_8[x][y][z] * 0.25 + wNoise_16[x][y][z] * 0.125;
				values[0] = remap(pNoise[x][y][z], 0, 1, values[1], 1);
				glTexSubImage3D(GL_TEXTURE_3D, 0, x, y, z, 1, 1, 1, GL_RGBA, GL_FLOAT, &values);
			}
		}
	}
	return cloudID;
}

GLuint Clouds::getDetailTexture_CPU() {

	unsigned int dimension = 32;
    const auto& range_dim = static_cast<int>(dimension);
	nGenerator->SetNoiseType(FastNoise::Cellular);
	nGenerator->SetCellularReturnType(FastNoise::Distance);
	nGenerator->SetFrequency(0.1);
	const auto& wNoise_1 = nGenerator->noiseVector3D(dimension, true);
	nGenerator->SetFrequency(0.2);
	const auto& wNoise_2 = nGenerator->noiseVector3D(dimension, true);
	nGenerator->SetFrequency(0.4);
	const auto& wNoise_4 = nGenerator->noiseVector3D(dimension, true);
	nGenerator->SetFrequency(0.8);
	const auto& wNoise_8 = nGenerator->noiseVector3D(dimension, true);
	nGenerator->SetFrequency(1);
	const auto& wNoise_16 = nGenerator->noiseVector3D(dimension, true);

	const auto& detailTexture = nTexture->generate3DTexture(dimension, dimension, dimension, GL_RGBA32F, GL_RGBA, GL_FLOAT);

	float values[4];
	for (int z = 0; z < range_dim; z++) {
		for (int y = 0; y < range_dim; y++) {
			for (int x = 0; x < range_dim; x++) {
				values[0] = wNoise_1[x][y][z] * 0.625 + wNoise_2[x][y][z] * 0.25 + wNoise_4[x][y][z] * 0.125;
				values[1] = wNoise_2[x][y][z] * 0.625 + wNoise_4[x][y][z] * 0.25 + wNoise_8[x][y][z] * 0.125;
				values[2] = wNoise_4[x][y][z] * 0.625 + wNoise_8[x][y][z] * 0.25 + wNoise_16[x][y][z] * 0.125;
				values[3] = 0;
				glTexSubImage3D(GL_TEXTURE_3D, 0, x, y, z, 1, 1, 1, GL_RGBA, GL_FLOAT, &values);
			}
		}
	}
	return detailTexture;
}

GLuint Clouds::getWeatherTexture_CPU() {

	unsigned int dimension = 512;
    const auto& range_dim = static_cast<int>(dimension);
	nGenerator->SetNoiseType(FastNoise::PerlinFractal);
	nGenerator->SetFrequency(0.015);
	nGenerator->SetFractalOctaves(5);
	nGenerator->SetFractalLacunarity(1.8);
	nGenerator->SetFractalGain(0.5);
	const auto& pNoise = nGenerator->noiseVector2D(dimension, false);
	const auto& weatherTexture = nTexture->generate2DTexture(dimension, dimension, GL_RGBA32F, GL_RGBA, GL_FLOAT);

	float values[4];
	for (int y = 0; y < range_dim; y++) {
		for (int x = 0; x < range_dim; x++) {
			values[0] = pNoise[x][y];
			glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, 1, 1, GL_RGBA, GL_FLOAT, &values);
		}
	}
	return weatherTexture;
}

void Clouds::getCloudTexture_GPU(bool create) {
	int dimension = 128;

	if (create) m_cloudTexture = nTexture->generate3DTexture(dimension, dimension, dimension, GL_RGBA8, GL_RGBA, GL_FLOAT);

	glUseProgram(m_perlinWorleyCompID);
	glBindImageTexture(0, m_cloudTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	glDispatchCompute(dimension, dimension, dimension);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glUseProgram(0);
}

void Clouds::getDetailTexture_GPU(bool create) {
	int dimension = 32;
	if (create) m_detailTexture = nTexture->generate3DTexture(dimension, dimension, dimension, GL_RGBA8, GL_RGBA, GL_FLOAT);

	glUseProgram(m_worleyCompID);
	glBindImageTexture(0, m_detailTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	glDispatchCompute(dimension, dimension, dimension);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glUseProgram(0);
}

void Clouds::getWeatherTexture_GPU(bool create) {
	int dimension = 512;
	if (create) m_weatherTexture = nTexture->generate2DTexture(dimension, dimension, GL_RGBA8, GL_RGBA, GL_FLOAT);

	glUseProgram(m_weatherCompID);
	glBindImageTexture(0, m_weatherTexture, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
    glUniform1f(m_uniform_weather_perlinAmplitude, m_weather_perlinAmplitude);
    glUniform1f(m_uniform_weather_perlinFrequency, m_weather_perlinFrecuency);
    glUniform1f(m_uniform_weather_perlinScale, m_weather_perlinScale);
    glUniform1i(m_uniform_weather_perlinOctaves, m_weather_perlinOctaves);
	glDispatchCompute(dimension, dimension, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glUseProgram(0);
}

GLuint Clouds::getMotionTexture() {
	int dimension = 128;
	int channels = 4;
	return nTexture->getTexture("../Resources/curlNoise.png", dimension, channels, GL_RGBA32F, GL_RGBA, GL_FLOAT);
}

GLuint Clouds::getBlueNoiseTexture() {
	int dimension = 256;
	int channels = 3;
	return nTexture->getTexture("../Resources/blueNoise.png", dimension, channels, GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE);
}

float Clouds::remap(float x, float a, float b, float c, float d)
{
	return (((x - a) / (b - a)) * (d - c)) + c;
}