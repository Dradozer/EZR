//
// Created by Dradozer on 17.05.2020.
//

#include "Terrain.h"

Terrain::Terrain(Window *window, sceneInfo *scene, const int seed, int dimensions) : m_window(window), m_scene(scene),
                                                                                     m_seed(seed),
                                                                                     m_chunkDimensions(dimensions),
                                                                                     m_patchSize(16), m_chunkSizeMult(4)
{
//    GLint MaxPatchVertices = 0;
//    glGetIntegerv(GL_MAX_PATCH_VERTICES, &MaxPatchVertices);
//    printf("Max supported patch vertices %d\n", MaxPatchVertices);

//    glPatchParameteri(GL_PATCH_VERTICES, 3);

    m_chunkSize = m_patchSize * m_chunkSizeMult;
    std::cout << "init with \nSeed: \t\t\t" << m_seed << std::endl;
    std::cout << "Dimensions:\t\t" << m_chunkDimensions << " x " << m_chunkDimensions << std::endl;
    std::cout << "Vertices: \t\t" << m_patchSize * m_patchSize * m_chunkDimensions * m_chunkDimensions << std::endl;

    ezr::Shader::attachShader(m_renderID, GL_VERTEX_SHADER, "vertexShader.glsl");
    ezr::Shader::attachShader(m_renderID, GL_TESS_CONTROL_SHADER,  "terrainTesCon.glsl");
    ezr::Shader::attachShader(m_renderID, GL_TESS_EVALUATION_SHADER,  "terrainTesEv.glsl");
    ezr::Shader::attachShader(m_renderID, GL_FRAGMENT_SHADER, "fragmentShader.glsl");

    ezr::Shader::linkShader(m_renderID);

    m_uniform_camPos = glGetUniformLocation(m_renderID, "cameraPos");
    m_uniform_viewM = glGetUniformLocation(m_renderID, "viewMatrix");
    m_uniform_projM = glGetUniformLocation(m_renderID, "projMatrix");
    m_lightPos = glGetUniformLocation(m_renderID, "lightPos");
    m_displaceFactor = glGetUniformLocation(m_renderID, "displaceFactor");
    m_eyePos = glGetUniformLocation(m_renderID, "gEyeWorldPos");

    setUniforms(reinterpret_cast<GLuint *>(m_renderID));

}

Terrain::~Terrain()
{
    ezr::Shader::deleteShader(m_renderID);
}

void Terrain::setUniforms(GLuint *id)
{
    glUseProgram(reinterpret_cast<GLuint>(id)); //one time setup for glUniforms
    {
        glUniform3fv(m_uniform_camPos, 1, glm::value_ptr(m_scene->cam->Position));
        glUniformMatrix4fv(m_uniform_viewM, 1, GL_FALSE, glm::value_ptr(m_scene->cam->GetViewMatrix()));
        glUniformMatrix4fv(m_uniform_projM, 1, GL_FALSE, glm::value_ptr(m_scene->projMatrix));
    }
    glUseProgram(0);
}

void Terrain::render()
{
    // render Chunk
    glUseProgram(m_renderID);
    glUniformMatrix4fv(m_uniform_viewM, 1, GL_FALSE, glm::value_ptr(m_scene->cam->GetViewMatrix()));
    glUniform3fv(m_lightPos, 1, glm::value_ptr(m_scene->lightPos));
    glUniform3fv(m_eyePos, 1, glm::value_ptr(m_scene->cam->Position));
    glUniform1f(m_displaceFactor, 0.25f);
    //uniforms für Tess
    m_window->isWireframeActive() ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK,
                                                                                              GL_FILL);

    glm::ivec2 currentChunk = glm::ivec2(round(m_scene->cam->Position.x / m_chunkSize),
                                         round(m_scene->cam->Position.z / m_chunkSize));
    for (int x = -m_chunkDimensions; x <= m_chunkDimensions; x++)
    {
        for (int z = -m_chunkDimensions; z <= m_chunkDimensions; z++)
        {
            glm::ivec2 viewedChunk = glm::ivec2(currentChunk.x + x, currentChunk.y + z);
            if (!contains(viewedChunk)) // contains is available in c++20, so i needed to write one myself
            {
                m_chunkDictionary.insert(std::pair<glm::ivec2, Chunk>(glm::ivec2(viewedChunk.x, viewedChunk.y),
                                                                      Chunk(m_chunkSizeMult, m_patchSize, viewedChunk.x,
                                                                            viewedChunk.y,
                                                                            m_seed)));
            }
            m_chunkDictionary.find(viewedChunk)->second.render();
        }
    }

    glUseProgram(0);
}

Camera *Terrain::getCamera() const
{
    return m_scene->cam;
}

bool Terrain::contains(glm::ivec2 viewedChunk)
{
    return m_chunkDictionary.count(viewedChunk) > 0;
}