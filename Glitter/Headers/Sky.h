#pragma once
#include "glitter.hpp"
#include "Texture.h"
#include "ezr_shader.h"
#include "../Sources/GUI/include/baseObject.h"

static const GLfloat m_triangles[] = {
	-1.0f, -1.0f, 0.0f,
	-1.0f, 1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
	-1.0f, -1.0f, 0.0f,
	1.0f, -1.0f, 0.0f,
	1.0f,  1.0f, 0.0f,
};

class Sky : public baseObject
{
public:
	Sky(int windowWidth, int windowHeight);
	~Sky();
	GLuint getTextureID();
	void render();
	virtual void setGui();
	virtual void draw();
	virtual void update();
private:
	Texture* nTexture;

	glm::vec3 m_skyColorTop;
	glm::vec3 m_skyColorBottom;

	GLuint m_uniform_invProjM;
	GLuint m_uniform_invViewM;
	GLuint m_uniform_skyColorTop;
	GLuint m_uniform_skyColorBottom;
	GLuint m_uniform_lightDirection;
	GLuint m_uniform_resolution;

	GLuint m_vertexbuffer;
	GLuint m_triangleID;
	GLuint m_computeID;
	GLuint m_screenSpaceID;
	GLuint m_textureID;
};