//
// Created by mhun on 02.06.20.
//

#ifndef GLITTER_COMPUTESHADER_H
#define GLITTER_COMPUTESHADER_H

#include <glad/glad.h>
#include "../Sources/GUI/include/baseObject.h"


class ComputeShader : public baseObject {
public:
    ComputeShader(GLuint cloudTex,GLuint detailTex,GLuint motionTex,GLuint weatherTex,GLuint currTex,GLuint prevTex,GLuint skyTex,GLuint blueTex);
    ~ComputeShader();
    virtual void setGui();
    virtual void draw();
    virtual void update();

    GLuint m_computeID;
    GLuint m_cloudTex;
    GLuint m_detailTex;
    GLuint m_motionTex;
    GLuint m_weatherTex;
    GLuint m_currTex;
    GLuint m_prevTex;
    GLuint m_skyTex;
    GLuint m_blueTex;
private:
    //////////////// STANDARD PARAMS //////////////////////////
    GLuint m_uniform_camPos;
    GLuint m_uniform_invProjM;
    GLuint m_uniform_invViewM;
    GLuint m_uniform_frame_time;
    GLuint m_uniform_resolution;
    //////////////// EARTH PARAMS //////////////////////////
    GLuint m_uniform_earth_rad;
    GLuint m_uniform_earth_pos;
    GLuint m_uniform_inner_atmo;
    GLuint m_uniform_outer_atmo;
    //////////////// SKY PARAMS //////////////////////////
    GLuint m_uniform_zenitColor;
    GLuint m_uniform_horizonColor;
    //////////////// LIGHT PARAMS //////////////////////////
    GLuint m_uniform_light_factor;
    GLuint m_uniform_realLightColor;
    GLuint m_uniform_lightDir;
    //////////////// CLOUD PARAMS //////////////////////////
    GLuint m_uniform_cloudColor;
    GLuint m_uniform_cloud_coverage;
    GLuint m_uniform_cloud_speed;
    GLuint m_uniform_cloud_crispiness;
    GLuint m_uniform_cloud_curliness;
    GLuint m_uniform_cloud_type;
    GLuint m_uniform_windDir;

    /////////////// TEXTURE PARAMS ////////////////////////
    GLuint m_uniform_baseNoiseScale;
    GLuint m_uniform_highFreqNoiseUVScale;
    GLuint m_uniform_maxRenderDist;
    ////////////////////////////////////////////////////////

    glm::vec3 m_earth_pos;
    glm::vec3 m_realLightColor;
    glm::vec3 m_cloudColor;
    glm::vec3 m_zenitColor;
    glm::vec3 m_horizonColor;
    glm::vec3 m_lightDir;
    glm::vec3 m_windDir;

    float m_earth_rad;
    float m_inner_atmo;
    float m_outer_atmo;

    float m_light_factor;
    float m_baseNoiseScale;
    float m_highFreqNoiseUVScale;
    float m_maxRenderDist;

    float m_cloud_coverage;
    float m_cloud_speed;
    float m_cloud_crispiness;
    float m_cloud_curliness;
    float m_cloud_type;

    int m_frameCounter;
    GLuint m_uniform_frameCounter;

    bool m_activateBlueNoise;
    GLuint m_uniform_bNoise;
};


#endif //GLITTER_COMPUTESHADER_H
