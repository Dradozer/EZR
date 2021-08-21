#include "Sky.h"

Sky::Sky(int windowWidth, int windowHeight) {

	nTexture = new Texture();

	ezr::Shader::attachShader(m_computeID, GL_COMPUTE_SHADER, "skyComp.glsl");
	ezr::Shader::linkShader(m_computeID);
	ezr::Shader::attachShader(m_screenSpaceID, GL_VERTEX_SHADER, "screenSpaceVert.glsl");
	ezr::Shader::attachShader(m_screenSpaceID, GL_FRAGMENT_SHADER , "screenSpaceFrag.glsl");
	ezr::Shader::linkShader(m_screenSpaceID);

	m_uniform_invProjM = glGetUniformLocation(m_computeID, "inv_proj");
	m_uniform_invViewM = glGetUniformLocation(m_computeID, "inv_view");
	m_uniform_skyColorTop = glGetUniformLocation(m_computeID, "skyColorTop");
	m_uniform_skyColorBottom = glGetUniformLocation(m_computeID, "skyColorBottom");
	m_uniform_lightDirection = glGetUniformLocation(m_computeID, "lightDirection");
	m_uniform_resolution = glGetUniformLocation(m_computeID, "resolution");

	m_skyColorTop = glm::vec3(0.5, 0.7, 0.8) * 1.05f;
	m_skyColorBottom = glm::vec3(0.9, 0.9, 0.95);

	m_textureID = nTexture->generate2DTexture(scene->resolution.x, scene->resolution.y, GL_RGBA8, GL_RGBA, GL_FLOAT);

	glGenVertexArrays(1, &m_triangleID);
	glBindVertexArray(m_triangleID);

	glGenBuffers(1, &m_vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(m_triangles), m_triangles, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

Sky::~Sky() {

}

GLuint Sky::getTextureID() {
	return m_textureID;
}

void Sky::update() {

}

void Sky::setGui() {

}

void Sky::draw() {

}

void Sky::render() {
	// generate sky texture in compute shader
	glUseProgram(m_computeID);
	glUniformMatrix4fv(m_uniform_invViewM, 1, GL_FALSE, glm::value_ptr(inverse(scene->cam->GetViewMatrix())));
	glUniformMatrix4fv(m_uniform_invProjM, 1, GL_FALSE, glm::value_ptr(inverse(scene->projMatrix)));
	glUniform2fv(m_uniform_resolution, 1, glm::value_ptr(scene->resolution));
	glUniform3fv(m_uniform_lightDirection, 1, glm::value_ptr(scene->lightDir));
	glUniform3fv(m_uniform_skyColorTop, 1, glm::value_ptr(m_skyColorTop));
	glUniform3fv(m_uniform_skyColorBottom, 1, glm::value_ptr(m_skyColorBottom));
	glBindImageTexture(0, m_textureID, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA8);
	glDispatchCompute(scene->resolution.x, scene->resolution.y, 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	glUseProgram(0);

	// render it on screen filling quad
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, m_textureID);
	//glUseProgram(m_screenSpaceID);
	//glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
	//glUseProgram(0);
	//glBindTexture(GL_TEXTURE_2D, 0);
}
