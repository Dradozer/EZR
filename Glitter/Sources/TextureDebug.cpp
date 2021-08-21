#include "TextureDebug.h"
#include <imgui.h>

TextureDebug::TextureDebug() {
}

TextureDebug::~TextureDebug() {
}

void TextureDebug::init(unsigned int windowHeight, unsigned int windowWidth) {

    m_currentTexture = 0;
    m_layer = 1;
    m_channel = 1;
    m_windowHeight = windowHeight;
    m_windowWidth = windowWidth;
    m_isActive = false;

    ezr::Shader::attachShader(m_renderTextureID, GL_VERTEX_SHADER, "texDebugVert.glsl");
    ezr::Shader::attachShader(m_renderTextureID, GL_FRAGMENT_SHADER, "texDebugFrag.glsl");
    ezr::Shader::linkShader(m_renderTextureID); // error occurs here

    m_uniform_is3D = glGetUniformLocation(m_renderTextureID, "is3D");
    m_uniform_layer = glGetUniformLocation(m_renderTextureID, "layer");
    m_uniform_channel = glGetUniformLocation(m_renderTextureID, "channel");
    m_uniform_dimension = glGetUniformLocation(m_renderTextureID, "dimension");

    glGenVertexArrays(1, &m_triangleID);
    glBindVertexArray(m_triangleID);

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_triangle), m_triangle, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
}

void TextureDebug::addTexture(GLuint textureID, int dimension, bool is3D) {
    m_textureID.push_back(textureID);
    m_dimension.push_back(dimension);
    m_is3D.push_back(is3D);
}

void TextureDebug::render() {
    if(m_textureID.size() == 0){
        return;
    }

    if (m_isActive) {
        glViewport(0, 0, m_windowHeight, m_windowHeight);
        if (!m_is3D[m_currentTexture]) {
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, m_textureID[m_currentTexture]);
        } else {
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_3D, m_textureID[m_currentTexture]);
        }
        glUseProgram(m_renderTextureID);
        glUniform1i(m_uniform_is3D, m_is3D[m_currentTexture]);
        glUniform1i(m_uniform_layer, m_layer);
        glUniform1i(m_uniform_channel, m_channel);
        glUniform1i(m_uniform_dimension, m_dimension[m_currentTexture]);
        //glDrawArrays(GL_TRIANGLE_STRIP, 0, 6);
        glUseProgram(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindTexture(GL_TEXTURE_3D, 0);
        glViewport(0, 0, m_windowWidth, m_windowHeight);
    }
}

void TextureDebug::setGui() {
    ImGui::Begin("Texture Debug");
    ImGui::Checkbox("Activate Texture Debugging", &m_isActive);
    if (m_isActive) {
        ImGui::Image((void*)(intptr_t)m_textureID[m_currentTexture], ImVec2(m_dimension[m_currentTexture], m_dimension[m_currentTexture]));
        ImGui::SliderInt("Select Texture", &m_currentTexture, 0, m_textureID.size()-1);
        ImGui::Text("OpenGL Texture ID: %d", m_textureID[m_currentTexture]);
        ImGui::Text("Dimension: %d", m_dimension[m_currentTexture]);
        ImGui::Text("3D: %s", (m_is3D[m_currentTexture]) ? "true" : "false");
        if (m_is3D[m_currentTexture]) ImGui::SliderInt("Layer", &m_layer, 0, m_dimension[m_currentTexture]-1);
        ImGui::SliderInt("Channel", &m_channel, 0, 4);
        if (ImGui::IsItemHovered())
            ImGui::SetTooltip("Channel 0 = Display all channels");
    }
    ImGui::End();
}

void TextureDebug::draw() {

}

void TextureDebug::update() {

}

void TextureDebug::clear(){
    m_textureID.clear();
    m_dimension.clear();
    m_is3D.clear();
}
