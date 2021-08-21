//
// Created by Dradozer on 17.05.2020.
//
#ifndef GLITTER_CHUNK_H
#define GLITTER_CHUNK_H

#include "iostream"
#include "glitter.hpp"
#include "Vertex.h"
#include <vector>
#include "noiseGenerator.h"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <random>

class Chunk
{
public:
    /**
     * Constructor of Chunk
     * @param chunkSizeMult determins how far away verts are depending on patchSize
     * @param patchSize determine the verticeNumber in both dimensions
     * @param x start vertex position.x, also functions as id in Terrain
     * @param z start vertex position.y, also functions as id in Terrain
     * @param seed of noise
     * calls gen() and setupMesh()
     */
    Chunk(const int chunkSizeMult, const int patchSize, const int x, const int z, const int seed);

    virtual ~Chunk();

    /**
     * render method of Chunk
     */
    void render();

private:
    /**
     * generates the Chunk and sets it up
     */
    void gen();

    /**
     * sets up the passing to the Shaders
     */
    void setupMesh();

    /**
     * generates all vertices
     */
    void genVertex();

    /**
     * calcs Normals, needs to be run after gen()
     * can be run alone
     */
    void calcNormals();

    /**
     * calcs Indices, needs to be run after gen()
     * can be run alone
     */
    void calcIndices();


    void calcTexCoords();
    int m_maxYValue;
    int m_x, m_z;
    int m_offsetX, m_offsetZ;
    FastNoise *m_noise;
    // m_patchSize: number of Vertices, m_chunkSizeMult: multiplicator for Vertice.Position, m_chunkSize: size of Chunk in 3D-Coord-Unit
    const int m_patchSize, m_chunkSizeMult;
    int m_chunkSize;
    int m_vertexCount;
    int m_indexCount;
    Vertex *m_tessVertices;
    std::vector<Vertex> m_vertices;
    std::vector<unsigned int> m_indices;

    GLuint VAO, VBO, EBO;
};

#endif //GLITTER_CHUNK_H
