//
// Created by Jakob on 01.08.2020.
//

#include "../Sources/GUI/include/baseObject.h"
#include "Texture.h"
#include "ezr_shader.h"

class ScreenQuad : public baseObject {
public:
    ScreenQuad();
    ~ScreenQuad();
    GLuint getTextureID();
    GLuint getProgramID();
    void render( GLuint m_textureID);
    virtual void setGui();
    virtual void draw();
    virtual void update();

//    void setGui() override;

    static void disableTests() {
        //glDisable(GL_CLIP_DISTANCE0);
        glDisable(GL_DEPTH_TEST);
    }

    static void enableTests() {
        glEnable(GL_DEPTH_TEST);
    }

    unsigned int quadVAO, quadVBO;
    bool initialized=false;
private:

    void initializeQuad();

    Texture* nTexture;


    GLuint m_uniform_skyColorTop;
    GLuint m_uniform_skyColorBottom;
    GLuint m_vertexbuffer;
    GLuint m_triangleID;
    GLuint m_screenSpaceID;
    GLuint m_textureID;
};
