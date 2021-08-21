//
// Created by mhun on 09.05.20.
//

#ifndef GLITTER_BASEOBJECT_H
#define GLITTER_BASEOBJECT_H

#include "sceneInfo.h"

class baseObject {
public:
    virtual void draw() = 0;
    virtual void setGui()  {};
    virtual void update() {};

    static sceneInfo* scene;
};


#endif //GLITTER_BASEOBJECT_H
