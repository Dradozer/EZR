//
// Created by mhun on 09.05.20.
//

#include "include/exampleObject.h"
#include <imgui.h>

exampleObject::exampleObject() {
    exampleVec = glm::vec3(1,0,0);
}

void exampleObject::setGui() {
    ImGui::Begin("exampleObject");
    ImGui::TextColored(ImVec4(1, 1, 0, 1), "example Colors");
    ImGui::ColorEdit3("Sky bottom color", (float*)&exampleVec); // Edit 3 floats representing a color
    ImGui::End();
}

void exampleObject::draw() {

}

void exampleObject::update() {

}

exampleObject::~exampleObject() {}