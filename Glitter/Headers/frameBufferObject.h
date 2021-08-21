#pragma once


#include <vector>

class frameBufferObject {
public:
    void bindFB(int fb, int w, int h);

    void unbindCurrentFB();

    void unbindCurrentFB(int Width, int Height);

    void activateTex(int i);

    void createFB();

    unsigned int createTexAttachment(int w, int h);

    unsigned int *createColAttachments(int w, int h, unsigned int nColAttachments);

    unsigned int createDepthTexAttachment(int w, int h);

    unsigned int createDepthBufferAttachment(int w, int h);

    unsigned int createRenderBufferAttachment(int w, int h);

    frameBufferObject(int w, int h);

    frameBufferObject(int w, int h, int numColorAttachments);

    unsigned int m_TFBO,m_CFBO, renderBuffer, depthTex, terrainTex, terrainDepthTex, cloudTex;

    unsigned int getColAttachmentTex(int i);

    void bindTFBO();
    void bindCFBO();

private:
    int m_w, m_h;
    int numColorAttachments;
    std::vector<unsigned int> m_TexIndices;
    unsigned int *colAttachments;
};

