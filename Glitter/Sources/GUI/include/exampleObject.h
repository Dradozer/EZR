//
// Created by mhun on 09.05.20.
//

#ifndef GLITTER_EXAMPLEOBJECT_H
#define GLITTER_EXAMPLEOBJECT_H
#include "baseObject.h"

class exampleObject : public baseObject{
public:

    exampleObject();
    ~exampleObject();
    virtual void setGui();
    virtual void draw();
    virtual void update();

private:
    glm::vec3 exampleVec;
};


#endif //GLITTER_EXAMPLEOBJECT_H
