//
// Created by mhun on 09.05.20.
//

#ifndef GLITTER_SCENEINFO_H
#define GLITTER_SCENEINFO_H

#include "camera.h"
#include <glm/glm.hpp>

struct sceneInfo{
    glm::vec3 lightPos, lightColor, lightDir, fogColor, seed;
    glm::mat4 projMatrix;
    Camera * cam;
    //FrameBufferObject * sceneFBO;
    bool wireframe = false;
};
#endif //GLITTER_SCENEINFO_H
