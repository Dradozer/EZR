//
// Created by mhun on 02.06.20.
//

#include "ComputeShader.h"
#include <imgui.h>
#include "ezr_shader.h"
#include "math.h"

#define PI 3.1415926535

ComputeShader::ComputeShader(GLuint cloudTex,GLuint detailTex,GLuint motionTex,GLuint weatherTex,GLuint currTex, GLuint prevTex,GLuint skyTex,GLuint blueTex) {

    m_cloudTex = cloudTex;
    m_detailTex =  detailTex;
    m_motionTex = motionTex;
    m_weatherTex = weatherTex;
    m_currTex = currTex;
    m_skyTex = skyTex;
    m_blueTex = blueTex;
    m_activateBlueNoise = false;
    m_frameCounter = 0;

    ezr::Shader::attachShader(m_computeID, GL_COMPUTE_SHADER,  "cloudsComp.glsl");
    ezr::Shader::linkShader(m_computeID);

    //////////////// STANDARD PARAMS //////////////////////////
    m_uniform_camPos = glGetUniformLocation(m_computeID, "cameraPos");
    m_uniform_invProjM = glGetUniformLocation(m_computeID, "inv_proj");
    m_uniform_invViewM = glGetUniformLocation(m_computeID, "inv_view");
    m_uniform_frame_time = glGetUniformLocation(m_computeID, "frame_time");
    m_uniform_resolution = glGetUniformLocation(m_computeID, "resolution");
    m_uniform_frameCounter = glGetUniformLocation(m_computeID, "frameCounter");
    //////////////// EARTH PARAMS //////////////////////////
    m_uniform_earth_rad = glGetUniformLocation(m_computeID, "earth_radius");
    m_uniform_earth_pos = glGetUniformLocation(m_computeID, "earth_pos");
    m_uniform_inner_atmo = glGetUniformLocation(m_computeID, "inner_atmosphere");
    m_uniform_outer_atmo = glGetUniformLocation(m_computeID, "outer_atmosphere");

    //////////////// SKY PARAMS //////////////////////////
    m_uniform_zenitColor = glGetUniformLocation(m_computeID, "zenitColor");
    m_uniform_horizonColor = glGetUniformLocation(m_computeID, "horizonColor");     

    //////////////// LIGHT PARAMS //////////////////////////
    m_uniform_light_factor = glGetUniformLocation(m_computeID, "lightFactor");;
    m_uniform_realLightColor = glGetUniformLocation(m_computeID, "realLightColor");
    m_uniform_lightDir = glGetUniformLocation(m_computeID, "lightDir");

    //////////////// CLOUD PARAMS //////////////////////////
    m_uniform_cloudColor = glGetUniformLocation(m_computeID, "cloudColor");
    m_uniform_cloud_coverage = glGetUniformLocation(m_computeID, "cloud_coverage");
    m_uniform_cloud_speed = glGetUniformLocation(m_computeID, "cloud_speed");
    m_uniform_cloud_crispiness = glGetUniformLocation(m_computeID,"cloud_crispiness");
    m_uniform_cloud_curliness = glGetUniformLocation(m_computeID,"cloud_curliness");
    m_uniform_cloud_type = glGetUniformLocation(m_computeID, "cloud_type");
    m_uniform_windDir = glGetUniformLocation(m_computeID, "windDirection");

    /////////////// TEXTURE PARAMS ////////////////////////

    m_uniform_baseNoiseScale = glGetUniformLocation(m_computeID, "baseNoiseScale");
    m_uniform_highFreqNoiseUVScale = glGetUniformLocation(m_computeID, "highFreqNoiseUVScale");
    m_uniform_maxRenderDist = glGetUniformLocation(m_computeID, "maxRenderDist");
    m_uniform_bNoise = glGetUniformLocation(m_computeID, "activateBNoise");

    ///////////////////////////////////////////////////////

    m_earth_pos = glm::vec3(0.0,0.0,0.0);
    m_earth_rad = 600000.f; // suggestion 6371km
    m_inner_atmo = 5000.f; //paper suggests values of 15000-35000m above
    m_outer_atmo = 17000.0;

    m_cloudColor = glm::vec3(1,1,1);
    m_cloud_coverage = 0.35f;
    m_cloud_speed = 400.f;
    m_cloud_crispiness = 35.f;
    m_cloud_curliness = 1.0f;
    m_cloud_type = 1.0f;

    m_light_factor = 1.0f;
    m_baseNoiseScale = 1.0f;
    m_highFreqNoiseUVScale = 150.f;

    m_realLightColor = glm::vec3(1,1,1);

    m_zenitColor = glm::vec3(0.0,0.5,1.0);
    m_horizonColor = glm::vec3(0.8,0.85,1.0);

    m_lightDir = scene->lightDir;
    m_windDir = glm::vec3(1.0,0.0,0.0);
    m_maxRenderDist = 6000000.0f;
}

void ComputeShader::setGui() {
    ImGui::Begin("Settings");

    //ImGui::Checkbox("le blue noise", &m_activateBlueNoise);

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Earth Control");
    ImGui::SliderFloat("Earth Radius", &m_earth_rad, 10000.0f, 5000000.0f);
    ImGui::SliderFloat3("Earth Position", (float*)&m_earth_pos, 0, 100);
    ImGui::SliderFloat("Inner Atmosphere", &m_inner_atmo, 0, 15000.0f);
    ImGui::SliderFloat("Outer Atmosphere", &m_outer_atmo, 100.0f, 40000.0f);

    //ImGui::TextColored(ImVec4(1, 1, 0, 1), "Sky");
    //ImGui::SliderFloat3("Zenit Color", (float*)&m_zenitColor, 0, 1);
    //ImGui::SliderFloat3("Horizon Color", (float*)&m_horizonColor, 0, 1);

    //ImGui::TextColored(ImVec4(1, 1, 0, 1), "Light");

    //ImGui::SliderFloat3("Light Color", (float*)&m_realLightColor, 0, 1);
    //ImGui::SliderFloat3("Light Direction", (float*)&scene->lightDir, 0, 1);
    //ImGui::SliderFloat("Light Factor", &m_light_factor, 0, 1);

    ImGui::TextColored(ImVec4(1, 1, 0, 1), "Cloud");
    ImGui::SliderFloat3("Cloud Color", (float*)&m_cloudColor, 0, 1);
    ImGui::SliderFloat("Cloud Coverage", &m_cloud_coverage, 0.0, 1.0);
    ImGui::SliderFloat("Cloud Speed", &m_cloud_speed, 0.0, 5.0e3);
    ImGui::SliderFloat("Cloud Crispiness", &m_cloud_crispiness, 0.0, 120.f);
    ImGui::SliderFloat("Cloud Curliness", &m_cloud_curliness, 0.0, 3.f);
    ImGui::SliderFloat("Cloud Type", &m_cloud_type, 0.0, 3.f);
    ImGui::SliderFloat3("Wind Direction", (float*)&m_windDir, 0, 1.f);



    //ImGui::SliderFloat("Low Noise Scale", &m_baseNoiseScale, 0.0, 1.0);
    //ImGui::SliderFloat("High Noise Scale", &m_highFreqNoiseUVScale, 0.0, 1.0);
    //ImGui::SliderFloat("Render Distance", &m_maxRenderDist, 0.0, 1000.0);
    ImGui::End();
}

void ComputeShader::update() { 
    
}

void ComputeShader::draw(){
    GLenum glError;
    glUseProgram(m_computeID);

    glUniform3fv(m_uniform_camPos, 1, glm::value_ptr(scene->cam->Position));
    glUniformMatrix4fv(m_uniform_invViewM, 1, GL_FALSE, glm::value_ptr(inverse(scene->cam->GetViewMatrix())));
    glUniformMatrix4fv(m_uniform_invProjM, 1, GL_FALSE, glm::value_ptr(inverse(scene->projMatrix)));
    glUniform1f(m_uniform_frame_time, glfwGetTime());
    glUniform2fv(m_uniform_resolution, 1, glm::value_ptr(scene->resolution));
    glUniform1i(m_uniform_frameCounter, m_frameCounter);

    glUniform1f(m_uniform_earth_rad, m_earth_rad);
    glUniform3fv(m_uniform_earth_pos, 1, glm::value_ptr(m_earth_pos));
    glUniform1f(m_uniform_inner_atmo, m_inner_atmo);
    glUniform1f(m_uniform_outer_atmo, m_outer_atmo);

    glUniform3fv(m_uniform_zenitColor, 1, glm::value_ptr(m_zenitColor));
    glUniform3fv(m_uniform_horizonColor, 1, glm::value_ptr(m_horizonColor));

    glUniform3fv(m_uniform_lightDir, 1, glm::value_ptr(scene->lightDir));
    glUniform3fv( m_uniform_realLightColor, 1, glm::value_ptr(m_realLightColor));
    glUniform1f(m_uniform_light_factor, m_light_factor);

    glUniform3fv( m_uniform_cloudColor, 1, glm::value_ptr(m_cloudColor));
    glUniform1f(m_uniform_cloud_coverage, m_cloud_coverage);
    glUniform1f(m_uniform_cloud_speed, m_cloud_speed);
    glUniform1f(m_uniform_cloud_crispiness, m_cloud_crispiness);
    glUniform1f(m_uniform_cloud_curliness, m_cloud_curliness);
    glUniform1f(m_uniform_cloud_type, m_cloud_type);
    glUniform3fv(m_uniform_windDir, 1, glm::value_ptr(m_windDir));

    glUniform1f(m_uniform_baseNoiseScale, m_baseNoiseScale);
    glUniform1f(m_uniform_highFreqNoiseUVScale, m_highFreqNoiseUVScale);

    glUniform1i(m_uniform_bNoise, m_activateBlueNoise);


    //glBindImageTexture(0, m_currTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glBindImageTexture(0, scene->sceneFBO->terrainTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_3D, m_cloudTex);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, m_detailTex);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, m_motionTex);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, m_weatherTex);
    glActiveTexture(GL_TEXTURE5);
    glBindTexture(GL_TEXTURE_2D, m_skyTex);
    glActiveTexture(GL_TEXTURE6);
    glBindTexture(GL_TEXTURE_2D, m_prevTex);
    glActiveTexture(GL_TEXTURE7);
    glBindTexture(GL_TEXTURE_2D, scene->sceneFBO->terrainTex);

    glDispatchCompute(scene->resolution.x, scene->resolution.y, 1);

    glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

    m_frameCounter > 15 ? m_frameCounter = 0 : m_frameCounter++;
    if ((glError = glGetError()) != GL_NO_ERROR) {
//        auskommentiert weil annoying
//        std::cout << "skyboxshaderintern OpenGL-Error: " << glError << std::endl;
    }
}