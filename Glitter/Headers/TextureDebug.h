#pragma once
#include "glitter.hpp"
#include "ezr_shader.h"
#include "../Sources/GUI/include/baseObject.h"
#include <vector>

static const GLfloat m_triangle[] = {
	-1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,	
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
};

class TextureDebug : public baseObject
{
public:
	TextureDebug();
	~TextureDebug();
	void init(unsigned int windowHeight, unsigned int windowWidth);
	void addTexture(GLuint textureID, int dimension, bool is3D);
	void render();
	void clear();
	virtual void setGui();
	virtual void draw();
	virtual void update();
private:
	GLuint m_renderTextureID;
	GLuint m_uniform_is3D;
	GLuint m_uniform_layer;
	GLuint m_uniform_channel;
	GLuint m_uniform_dimension;
	GLuint m_triangleID;
	GLuint vertexbuffer;
	std::vector<GLuint> m_textureID;
	std::vector<int> m_dimension;
	std::vector<bool> m_is3D;
	bool m_displayChannels;
	bool m_isActive;
	int m_currentTexture;
	int m_layer;
	int m_channel;
	int m_windowHeight;
	int m_windowWidth;
};
