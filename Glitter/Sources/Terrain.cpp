//
// Created by Dradozer on 17.05.2020.
//

#include "Terrain.h"

Terrain::Terrain(Window *window, sceneInfo *scene, const int seed, int dimensions) : m_window(window), m_scene(scene),
                                                                                     m_seed(seed),
                                                                                     m_chunkDimensions(dimensions),
                                                                                     m_patchSize(8), m_chunkSizeMult(8)
{
    m_chunkSize = m_patchSize * m_chunkSizeMult;
    std::cout << "init with \nSeed: \t\t\t" << m_seed << std::endl;
    std::cout << "Dimensions:\t\t" << m_chunkDimensions << " x " << m_chunkDimensions << std::endl;
    std::cout << "Vertices: \t\t" << m_patchSize * m_patchSize * m_chunkDimensions * m_chunkDimensions << std::endl;

    ezr::Shader::attachShader(m_renderTerrainID, GL_VERTEX_SHADER, "vertexTerrainShader.glsl");
    ezr::Shader::attachShader(m_renderTerrainID, GL_TESS_CONTROL_SHADER,  "terrainTesCon.glsl");
    ezr::Shader::attachShader(m_renderTerrainID, GL_TESS_EVALUATION_SHADER,  "terrainTesEv.glsl");
    ezr::Shader::attachShader(m_renderTerrainID, GL_FRAGMENT_SHADER, "fragmentTerrainShader.glsl");

    ezr::Shader::linkShader(m_renderTerrainID);

    m_uniform_camPos = glGetUniformLocation(m_renderTerrainID, "cameraPos");
    m_uniform_viewM = glGetUniformLocation(m_renderTerrainID, "viewMatrix"); 
    m_uniform_projM = glGetUniformLocation(m_renderTerrainID, "projMatrix");
    m_lightPos = glGetUniformLocation(m_renderTerrainID, "lightPos");
    m_displaceFactor = glGetUniformLocation(m_renderTerrainID, "displaceFactor");
    m_eyePos = glGetUniformLocation(m_renderTerrainID, "gEyeWorldPos");
    m_chunkPos = glGetUniformLocation(m_renderTerrainID, "chunkPos");
    m_useed = glGetUniformLocation(m_renderTerrainID, "seed");
    m_lod_b = glGetUniformLocation(m_renderTerrainID, "LoD_b");
    m_lod_v4 = glGetUniformLocation(m_renderTerrainID, "LoD_v4");

    this->sandTex = nTexture->getTexture("../Resources/sand.jpg", 1024, 4, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    this->grassTex = nTexture->getTexture("../Resources/grass.jpg", 1024, 4, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    this->rockTex = nTexture->getTexture("../Resources/rdiffuse.jpg", 768, 4, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    this->snowTex = nTexture->getTexture("../Resources/snow.jpg", 1024, 4, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    this->normalTex = nTexture->getTexture("../Resources/rnormal.jpg", 768, 4, GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE);
    this->grass1Tex = nTexture->getTexture("../Resources/terrainTexture.jpg", 2048, 4, GL_RGBA, GL_RGBA,
                                           GL_UNSIGNED_BYTE);

    m_uniform_SandTextureLoc    = glGetUniformLocation(m_renderTerrainID, "sand");
    m_uniform_GrassTextureLoc   = glGetUniformLocation(m_renderTerrainID, "grass");
    m_uniform_RockTextureLoc = glGetUniformLocation(m_renderTerrainID, "rock");
    m_uniform_SnowTextureLoc    = glGetUniformLocation(m_renderTerrainID, "snow");
    m_uniform_Grass1TextureLoc = glGetUniformLocation(m_renderTerrainID, "grass1");
    m_uniform_rockNormalTextureLoc  = glGetUniformLocation(m_renderTerrainID, "rockNormal");

    setUniforms(m_renderTerrainID);

}

Terrain::~Terrain()
{
    ezr::Shader::deleteShader(m_renderTerrainID);
}

void Terrain::setUniforms(GLuint &id)
{
    glUseProgram(id); //one time setup for glUniforms
    {
        glUniform3fv(m_uniform_camPos, 1, glm::value_ptr(m_scene->cam->Position));
        glUniformMatrix4fv(m_uniform_viewM, 1, GL_FALSE, glm::value_ptr(m_scene->cam->GetViewMatrix()));
        glUniformMatrix4fv(m_uniform_projM, 1, GL_FALSE, glm::value_ptr(m_scene->projMatrix));
    }
    glUseProgram(0);
}

void Terrain::render()
{    GLenum glError;
    // render Chunk
    glUseProgram(m_renderTerrainID);
    glUniformMatrix4fv(m_uniform_viewM, 1, GL_FALSE, glm::value_ptr(m_scene->cam->GetViewMatrix()));
    glUniform3fv(m_lightPos, 1, glm::value_ptr(m_scene->lightPos));
    glUniform3fv(m_eyePos, 1, glm::value_ptr(m_scene->cam->Position));
    glUniform3fv(m_useed, 1, glm::value_ptr(m_scene->seed));
    glUniform1f(m_displaceFactor, 1.f);
    glUniform1i(m_lod_b, m_tesse);
    glUniform4fv(m_lod_v4, 1 ,glm::value_ptr( m_LoD ) );
    // set textures
    set2dTex( m_uniform_SandTextureLoc , sandTex ,1 );
    set2dTex( m_uniform_GrassTextureLoc , grassTex ,2 );
    set2dTex(m_uniform_RockTextureLoc , rockTex , 3 );
    set2dTex( m_uniform_SnowTextureLoc , snowTex ,4 );
    set2dTex(m_uniform_Grass1TextureLoc , grass1Tex , 5 );
    set2dTex(m_uniform_rockNormalTextureLoc , normalTex , 6 );
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
    }
    //uniforms für Tess
    m_window->isWireframeActive() ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE) : glPolygonMode(GL_FRONT_AND_BACK,
                                                                                              GL_FILL);

    glm::ivec2 currentChunk = glm::ivec2(ceil(m_scene->cam->Position.x / m_chunkSize),
                                         ceil(m_scene->cam->Position.z / m_chunkSize))-1;
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
            glUniform1f(m_chunkPos, ( abs(x) + abs(z) ) /2.f );
            m_chunkDictionary.find(viewedChunk)->second.render();
        }
    }

    glUseProgram(0);
}

void Terrain::setGui(){
    m_LoD = ImGui::Button("Reset") ? glm::vec4(20,10,5,3) : m_LoD;
    ImGui::SliderFloat("LoD_1", &m_LoD.x, 1.0f, 100.0f);
    ImGui::SliderFloat("LoD_2", &m_LoD.y, 1.0f, 100.0f);
    ImGui::SliderFloat("LoD_3", &m_LoD.z, 1.0f, 100.0f);
    ImGui::SliderFloat("LoD_4", &m_LoD.w, 1.0f, 100.0f);
    ImGui::Checkbox("Tesselation", &m_tesse);
}

void Terrain::draw(){}
void Terrain::update(){}

Camera *Terrain::getCamera() const
{
    return m_scene->cam;
}

bool Terrain::contains(glm::ivec2 viewedChunk)
{
    return m_chunkDictionary.count(viewedChunk) > 0;
}
void Terrain::set2dTex(  GLuint location, unsigned int texture, int id) const
{
    GLenum glError;

    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(location, id);

    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "TexturBinding at "<< id << " OpenGL-Error: " << glError << std::endl;
    }
}