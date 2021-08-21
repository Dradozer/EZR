
#include <glad/glad.h>
#include <iostream>
#include "frameBufferObject.h"

GLenum glError;

void frameBufferObject::bindFB(int fb, int w, int h) {
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "bindFB prefb OpenGL-Error: " << glError << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "bindFB fb OpenGL-Error: " << glError << std::endl;
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "bindFB tex OpenGL-Error: " << glError << std::endl;
    }
    glViewport(0, 0, w, h);
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "bindFB vp OpenGL-Error: " << glError << std::endl;
    }
}

void frameBufferObject::unbindCurrentFB() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, m_w, m_h);
}

void frameBufferObject::unbindCurrentFB(int Width, int Height) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, Width, Height);
}

void frameBufferObject::activateTex(int i) {
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, getColAttachmentTex(i), 0);
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << " switching tex OpenGL-Error: " << glError << std::endl;
    }
}

void frameBufferObject::createFB() {

    glGenFramebuffers(1, &m_TFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_TFBO);
    //Attach ColorTex
    glGenTextures(1, &terrainTex);
    glBindTexture(GL_TEXTURE_2D, terrainTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_w, m_h, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindImageTexture(0, terrainTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, terrainTex, 0);

    //depthComponent
    glGenTextures(1,&terrainDepthTex);
    glBindTexture(GL_TEXTURE_2D, terrainDepthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, m_w, m_h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, terrainDepthTex, 0);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Terrain FBO schrott " << glError << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glGenFramebuffers(1, &m_CFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, m_CFBO);
    //Attach ColorTex
    glGenTextures(1, &cloudTex);
    glBindTexture(GL_TEXTURE_2D, cloudTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, m_w, m_h, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindImageTexture(0, cloudTex, 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, cloudTex, 0);


    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Cloud FBO schrott " << glError << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

unsigned int frameBufferObject::createTexAttachment(int w, int h) {


    unsigned int textureFBO;
    glGenTextures(1, &textureFBO);

    glBindTexture(GL_TEXTURE_2D, textureFBO);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureFBO, 0);
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
    }

    return textureFBO;

}

unsigned int *frameBufferObject::createColAttachments(int w, int h, unsigned int nColAttachments) {
    GLenum glError;
    unsigned int *colAttachments = new unsigned int[nColAttachments];
    glGenTextures(nColAttachments, colAttachments);


    for (unsigned int i = 0; i < nColAttachments; i++) {
        glBindTexture(GL_TEXTURE_2D, colAttachments[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, w, h, 0, GL_RGBA, GL_FLOAT, NULL);
//        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, 0);
        glBindImageTexture(1, colAttachments[i], 0, GL_FALSE, 0, GL_READ_WRITE, GL_RGBA32F);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, colAttachments[i], 0);
        if ((glError = glGetError()) != GL_NO_ERROR) {
            std::cout << "Create n-Tex OpenGL-Error: " << glError << std::endl;
        }


    }
    return colAttachments;
}

unsigned int frameBufferObject::createDepthBufferAttachment(int w, int h) {
    unsigned int depthBuf;
    glGenRenderbuffers(1, &depthBuf);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuf);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuf);
    return depthBuf;
}

unsigned int frameBufferObject::createDepthTexAttachment(int w, int h) {

    unsigned int depthTex;
    glGenTextures(1, &depthTex);
    glBindTexture(GL_TEXTURE_2D, depthTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTex, 0);
    return depthTex;
}

unsigned int frameBufferObject::createRenderBufferAttachment(int w, int h) {
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w,
                          h); // use a single renderbuffer object for both a depth AND stencil buffer.
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
                              RBO); // now actually attach it

    return RBO;
}

frameBufferObject::frameBufferObject(int w, int h) {
    GLenum glError;
    m_h = h;
    m_w = w;
    createFB();
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "FBO  " << glError << std::endl;
    }
//    this->terrainTex = createTexAttachment(m_w, m_h);

    depthTex = createDepthTexAttachment(m_w, m_h);

    colAttachments = NULL;
    numColorAttachments = 1;
}

frameBufferObject::frameBufferObject(int w, int h, int numColorAttachments) {
    GLenum glError;
    m_w = w;
    m_h = h;
    createFB();

//    terrainTex = NULL;
//    depthTex = createDepthTexAttachment(m_w, m_h);
//    colAttachments = createColAttachments(m_w, m_h, numColorAttachments);
//    this->numColorAttachments = numColorAttachments;
//
//    unsigned int *colAttachmentsFlag = new unsigned int[numColorAttachments];
//    for (unsigned int i = 0; i < numColorAttachments; i++) {
//        colAttachmentsFlag[i] = GL_COLOR_ATTACHMENT0 + i;
//    }
//    glDrawBuffers(numColorAttachments, colAttachmentsFlag);
//    delete colAttachmentsFlag;

}

unsigned int frameBufferObject::getColAttachmentTex(int i) {
    unsigned int colTex;
    if (i < 0 || i > numColorAttachments) {
        std::cout << "Col-Attachment out of Range" << std::endl;
        return 0;
    }

    return colAttachments[i];
}

void frameBufferObject::bindTFBO() {
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "pre-bindTFBo OpenGL-Error: " << glError << std::endl;
    }
    bindFB(this->m_TFBO, m_w, m_h);
}

void frameBufferObject::bindCFBO() {
    if ((glError = glGetError()) != GL_NO_ERROR) {
        std::cout << "pre-bindCFBO  OpenGL-Error: " << glError << std::endl;
    }
    bindFB(this->m_CFBO, m_w, m_h);
}
