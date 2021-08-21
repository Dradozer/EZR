//
// Created by Jakob on 01.08.2020.
//

#include <ScreenQuad.h>
#include "ezr_shader.h"

static const GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,
};
ScreenQuad::ScreenQuad() {



    GLenum glError;
    initializeQuad();
    ezr::Shader::attachShader(m_screenSpaceID, GL_VERTEX_SHADER, "screenVert.glsl");
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Sscree1 " << glError << std::endl;
    }
    ezr::Shader::attachShader(m_screenSpaceID, GL_FRAGMENT_SHADER, "visualizeFboFrag.glsl");
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Sscree2 validation OpenGL-Error: " << glError << std::endl;
    }
//    ezr::Shader::attachShader(m_screenSpaceID, GL_VERTEX_SHADER, "screenSpaceVert.glsl");
//    ezr::Shader::attachShader(m_screenSpaceID, GL_FRAGMENT_SHADER, "screenSpaceFrag.glsl");
    ezr::Shader::linkShader(m_screenSpaceID);

//    m_textureID = nTexture->generate2DTexture(scene->resolution.x, scene->resolution.y, GL_RGBA8, GL_RGBA, GL_FLOAT);
//    if ((glError = glGetError()) != GL_NO_ERROR) {
//        std::cout << "Sscree3 validation OpenGL-Error: " << glError << std::endl;
//    }
//    glGenVertexArrays(1, &m_triangleID);
//    glBindVertexArray(m_triangleID);
//
//    glGenBuffers(1, &m_vertexbuffer);
//    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
//    glBufferData(GL_ARRAY_BUFFER, sizeof(m_triangles), m_triangles, GL_STATIC_DRAW);
//
//    glEnableVertexAttribArray(0);
//    glBindBuffer(GL_ARRAY_BUFFER, m_vertexbuffer);
//    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void *) 0);
//    if ((glError = glGetError()) != GL_NO_ERROR) {
//        std::cout << "screenquad: " << glError << std::endl;
//    }
}

void ScreenQuad::initializeQuad() {
    if (!initialized) {
        float vertices[] = {
                -1.0f, -1.0f, 0.0, 0.0,
                1.0f, -1.0f, 1.0, 0.0,
                -1.0f, 1.0f, 0.0, 1.0,
                1.0f, 1.0f, 1.0, 1.0,
                -1.0f, 1.0f, 0.0, 1.0,
                1.0f, -1.0f, 1.0, 0.0
        };
        GLenum glError;
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        if ((glError = glGetError()) != GL_NO_ERROR) {
            std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
        }

        ScreenQuad::initialized = true;
    }

}

GLuint ScreenQuad::getTextureID() {
    return m_textureID;
};

GLuint ScreenQuad::getProgramID() {
    return m_screenSpaceID;
};


void ScreenQuad::render(GLuint m_textureID) {
    GLenum glError;
// render it on screen filling quad
    glDisable(GL_DEPTH_TEST);
    glUseProgram(m_screenSpaceID);
//    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(this->quadVAO);
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
    }
    glDrawArrays(GL_TRIANGLES, 0, 6);

    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
    }
    glUseProgram(0);
//    if ((glError = glGetError()) != GL_NO_ERROR) {
//        std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
//    }
//    glBindTexture(GL_TEXTURE_2D, 0);
//    if ((glError = glGetError()) != GL_NO_ERROR) {
//        std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
//    }
}

void ScreenQuad::update() {

}

void ScreenQuad::setGui() {

}

void ScreenQuad::draw() {

}