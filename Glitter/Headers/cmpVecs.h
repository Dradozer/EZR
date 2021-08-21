//
// Created by Dradozer on 30.05.2020.
//

#ifndef GLITTER_CMPVECS_H
#define GLITTER_CMPVECS_H

#include <glm/glm.hpp>

struct cmpVecs{
    bool operator()(const glm::ivec2& a, const glm::ivec2& b) const {
        return (((std::int64_t)a.x << 30) + a.y) < (((std::int64_t)b.x << 30) + b.y);
    }
};


#endif //GLITTER_CMPVECS_H
