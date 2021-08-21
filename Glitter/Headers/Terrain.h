//
// Created by Dradozer on 17.05.2020.
//

#ifndef GLITTER_TERRAIN_H
#define GLITTER_TERRAIN_H

// headers
#include "GUI/include/sceneInfo.h"
#include "GUI/include/Window.h"
#include <iostream>
#include <vector>
#include <map>

#include "EZR_Shader/ezr_shader.h"
#include "EZR_Shader/ezr_ssbo.h"
#include "EZR_Shader/ezr_computeShader.h"

#include "cmpVecs.h"
#include "Chunk.h"


class Terrain
{
public:
    /**
     * Constructor of Terrain
     * @param camera
     * @param seed
     */

    Terrain(Window *window, sceneInfo *scene, const int seed = 42069, int dimensions = 20);

    virtual ~Terrain();

    void render();

    Camera *getCamera() const;

private:
    /**
     * is implemented in C++20, this is just a slow alternative
     */
    bool contains(glm::ivec2);

    /**
     * passes glUniforms to Shader
     * @param id ShaderID
     */
    void setUniforms(GLuint *id);

    /**
     * @param how many Chunk are generated around the Player
     */
    int m_chunkDimensions;

    Window *m_window;
    sceneInfo *m_scene;
    const int m_seed;
    // m_patchSize: number of Vertices, m_chunkSizeMult: multiplicator for Vertice.Position, m_chunkSize: size of Chunk in 3D-Coord-Unit
    const int m_patchSize, m_chunkSizeMult;
    int m_chunkSize;

    std::map<glm::ivec2, Chunk, cmpVecs> m_chunkDictionary;

    GLuint m_renderID;
    GLuint m_uniform_camPos;
    GLuint m_uniform_viewM;
    GLuint m_uniform_projM;
    //tesselationstuffadressen, kann im array gefasst werden
    GLuint m_lightPos;
    GLuint m_eyePos;
    GLuint m_displaceFactor;


};

#endif //GLITTER_TERRAIN_H
