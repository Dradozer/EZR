//
// Created by Dradozer on 17.05.2020.
//

#ifndef GLITTER_VERTEX_H
#define GLITTER_VERTEX_H

#include <glm/glm.hpp>

struct Vertex {
    glm::vec3 Position;
    glm::vec3 Normal;
    glm::vec2 UVCoords;
    Vertex(glm::vec3 position, glm::vec3 normal, glm::vec2 uvCoords)
    {
        Position = position;
        Normal = normal;
        UVCoords = uvCoords;
    }
};

#endif //GLITTER_VERTEX_H
