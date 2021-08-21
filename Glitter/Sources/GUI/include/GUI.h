//
// Created by mhun on 09.05.20.
//

#ifndef GLITTER_GUI_H
#define GLITTER_GUI_H

#pragma once
#include "Window.h"
#include "baseObject.h"

#include <algorithm>
#include <list>

class GUI : public baseObject
{
public:
    GUI(Window& w);
    ~GUI();

    virtual void draw();
    virtual void update();

    GUI& subscribe(baseObject* subscriber);

private:
    // the other drawableObjects expose their setGui() methods (because he let the user handle their own attributes), so they can be subscribed to the GUI class
    // the GUI class will call the setGui() method for each subscriber
    std::list<baseObject*> subscribers;

};

#endif //GLITTER_GUI_H