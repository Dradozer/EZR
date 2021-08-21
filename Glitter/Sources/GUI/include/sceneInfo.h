//
// Created by mhun on 09.05.20.
//

#ifndef GLITTER_SCENEINFO_H
#define GLITTER_SCENEINFO_H

#include "camera.h"
#include <glm/glm.hpp>
#include <frameBufferObject.h>

struct sceneInfo{
    glm::vec3 lightPos, lightColor, lightDir, fogColor, seed, rockColor;
    glm::vec2 resolution;
    glm::mat4 projMatrix;
    Camera * cam;
    frameBufferObject * sceneFBO;
    bool wireframe = false;

};
#endif //GLITTER_SCENEINFO_H
