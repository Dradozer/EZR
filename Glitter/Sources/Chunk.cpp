//
// Created by Dradozer on 17.05.2020.
//

#include "Chunk.h"

Chunk::Chunk(const int chunkSizeMult, const int patchSize, const int x, const int z, const int seed) : m_chunkSizeMult(
        chunkSizeMult), m_patchSize(patchSize), m_x(x), m_z(z)
{
    m_chunkSize = (m_chunkSizeMult * m_patchSize);
    m_offsetX = m_x * (m_chunkSize - 1 * m_chunkSizeMult);
    m_offsetZ = m_z * (m_chunkSize - 1 * m_chunkSizeMult);
    m_maxYValue = 200;
    m_vertexCount = m_patchSize * m_patchSize;
    m_noise = new FastNoise;
    m_noise->SetNoiseType(FastNoise::PerlinFractal);
    m_noise->SetSeed(seed);
    gen();
    setupMesh();
}

Chunk::~Chunk()
{

}

void Chunk::gen()
{
//    Vertex *m_tessVertices = new Vertex[m_vertexCount];

    genVertex();

    calcIndices();

    calcNormals();
}

void Chunk::setupMesh()
{

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

    // vertex positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Position));

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *) offsetof(Vertex, Normal));

    glBindVertexArray(0);
}

void Chunk::render()
{
    //test tesselationstuff
    //create something to render

    glBindVertexArray(VAO);
    glDrawElements(GL_PATCHES, m_indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Chunk::genVertex()
{
    m_vertices.clear();
    for (int x = 0; x < m_patchSize; x++)
    {
        for (int z = 0; z < m_patchSize; z++)
        {
//    Vertex temp = Vertex(glm::vec3(x , m_noise->GetNoise(x,z) * m_maxYValue, z), glm::vec3(0, 0, 0), glm::vec2(0, 0));
            m_vertices.push_back(
                    Vertex(glm::vec3(m_chunkSizeMult * x + m_offsetX, m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX,
                                                                                        m_chunkSizeMult * z +
                                                                                        m_offsetZ) * m_maxYValue,
                                     m_chunkSizeMult * z + m_offsetZ), glm::vec3(0, 0, 0), glm::vec2(0, 0)));
        }
    }
//    std::cout << "vert bla " << m_vertices.size() << std::endl;
    m_vertexCount = m_vertices.size();
}

void Chunk::calcNormals()
{   // von 8.613 ms zu 0.939 ms for 64 *64
    int i = m_patchSize + 1;
    for (int x = 1; x < m_patchSize - 1; x++)
    {
        for (int z = 1; z < m_patchSize - 1; z++)
        {
            glm::vec3 Vt01 = m_vertices[i - m_patchSize].Position + m_vertices[i].Position;
            glm::vec3 Vt02 = m_vertices[i].Position + m_vertices[i + m_patchSize].Position;

            glm::vec3 Vt03 = m_vertices[i - 1].Position + m_vertices[i].Position;
            glm::vec3 Vt04 = m_vertices[i].Position + m_vertices[i + 1].Position;

            glm::vec3 tangent0 = Vt01 - Vt02;
            glm::vec3 tangent1 = Vt03 - Vt04;
            glm::vec4(glm::normalize(glm::cross(tangent0, tangent1)), 0);

            m_vertices[i].Normal = glm::normalize(glm::cross(tangent0, tangent1));
            i++;
        }
        i += 2;
    }

    for (int x = 0; x < m_patchSize; x += m_patchSize - 1)
    {
        i = m_patchSize * x;
        for (int z = 0; z < m_patchSize; z++)
        {
            glm::vec3 Vt01 = glm::vec3(m_chunkSizeMult * x - 1 + m_offsetX,
                                       m_noise->GetNoise(m_chunkSizeMult * x - 1 + m_offsetX,
                                                         m_chunkSizeMult * z + m_offsetZ) * m_maxYValue,
                                       m_chunkSizeMult * z + m_offsetZ) +
                             glm::vec3(m_chunkSizeMult * x + m_offsetX,
                                       m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX,
                                                         m_chunkSizeMult * z + m_offsetZ) * m_maxYValue,
                                       m_chunkSizeMult * z + m_offsetZ);
            glm::vec3 Vt02 =
                    glm::vec3(m_chunkSizeMult * x + m_offsetX,
                              m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX, m_chunkSizeMult * z + m_offsetZ) *
                              m_maxYValue, m_chunkSizeMult * z + m_offsetZ) +
                    glm::vec3(m_chunkSizeMult * x + 1 + m_offsetX,
                              m_noise->GetNoise(m_chunkSizeMult * x + 1 + m_offsetX, m_chunkSizeMult * z + m_offsetZ) *
                              m_maxYValue,
                              m_chunkSizeMult * z + m_offsetZ);

            glm::vec3 Vt03 = glm::vec3(m_chunkSizeMult * x + m_offsetX,
                                       m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX,
                                                         m_chunkSizeMult * z - 1 + m_offsetZ) * m_maxYValue,
                                       m_chunkSizeMult * z - 1 + m_offsetZ) +
                             glm::vec3(m_chunkSizeMult * x + m_offsetX,
                                       m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX,
                                                         m_chunkSizeMult * z + m_offsetZ) * m_maxYValue,
                                       m_chunkSizeMult * z + m_offsetZ);
            glm::vec3 Vt04 =
                    glm::vec3(m_chunkSizeMult * x + m_offsetX,
                              m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX, m_chunkSizeMult * z + m_offsetZ) *
                              m_maxYValue, m_chunkSizeMult * z + m_offsetZ) +
                    glm::vec3(m_chunkSizeMult * x + m_offsetX,
                              m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX, m_chunkSizeMult * z + 1 + m_offsetZ) *
                              m_maxYValue,
                              m_chunkSizeMult * z + 1 + m_offsetZ);

            glm::vec3 tangent0 = Vt01 - Vt02;
            glm::vec3 tangent1 = Vt03 - Vt04;
            glm::vec4(glm::normalize(glm::cross(tangent0, tangent1)), 0);

            m_vertices[i].Normal = glm::normalize(glm::cross(tangent0, tangent1));
            i++;
        }
    }

    for (int x = 0; x < m_patchSize; x++)
    {
        i = m_patchSize * x;
        for (int z = 0; z < m_patchSize; z += m_patchSize - 1)
        {
            glm::vec3 Vt01 = glm::vec3(m_chunkSizeMult * x - 1 + m_offsetX,
                                       m_noise->GetNoise(m_chunkSizeMult * x - 1 + m_offsetX,
                                                         m_chunkSizeMult * z + m_offsetZ) * m_maxYValue,
                                       m_chunkSizeMult * z + m_offsetZ) +
                             glm::vec3(m_chunkSizeMult * x + m_offsetX,
                                       m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX,
                                                         m_chunkSizeMult * z + m_offsetZ) * m_maxYValue,
                                       m_chunkSizeMult * z + m_offsetZ);
            glm::vec3 Vt02 =
                    glm::vec3(m_chunkSizeMult * x + m_offsetX,
                              m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX, m_chunkSizeMult * z + m_offsetZ) *
                              m_maxYValue, m_chunkSizeMult * z + m_offsetZ) +
                    glm::vec3(m_chunkSizeMult * x + 1 + m_offsetX,
                              m_noise->GetNoise(m_chunkSizeMult * x + 1 + m_offsetX, m_chunkSizeMult * z + m_offsetZ) *
                              m_maxYValue,
                              m_chunkSizeMult * z + m_offsetZ);

            glm::vec3 Vt03 = glm::vec3(m_chunkSizeMult * x + m_offsetX,
                                       m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX,
                                                         m_chunkSizeMult * z - 1 + m_offsetZ) * m_maxYValue,
                                       m_chunkSizeMult * z - 1 + m_offsetZ) +
                             glm::vec3(m_chunkSizeMult * x + m_offsetX,
                                       m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX,
                                                         m_chunkSizeMult * z + m_offsetZ) * m_maxYValue,
                                       m_chunkSizeMult * z + m_offsetZ);
            glm::vec3 Vt04 =
                    glm::vec3(m_chunkSizeMult * x + m_offsetX,
                              m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX, m_chunkSizeMult * z + m_offsetZ) *
                              m_maxYValue, m_chunkSizeMult * z + m_offsetZ) +
                    glm::vec3(m_chunkSizeMult * x + m_offsetX,
                              m_noise->GetNoise(m_chunkSizeMult * x + m_offsetX, m_chunkSizeMult * z + 1 + m_offsetZ) *
                              m_maxYValue,
                              m_chunkSizeMult * z + 1 + m_offsetZ);

            glm::vec3 tangent0 = Vt01 - Vt02;
            glm::vec3 tangent1 = Vt03 - Vt04;
            glm::vec4(glm::normalize(glm::cross(tangent0, tangent1)), 0);

            m_vertices[i].Normal = glm::normalize(glm::cross(tangent0, tangent1));
            i += m_patchSize - 1;
        }
    }
}

void Chunk::calcIndices()
{
    m_indices.clear();
    int i = 0;

    for (int x = 0; x < m_patchSize - 1; x++)
    {
        for (int z = 0; z < m_patchSize - 1; z++)
        {
            //1 triangle ccw
            m_indices.push_back(i + 0);
            m_indices.push_back(i + m_patchSize + 1);
            m_indices.push_back(i + m_patchSize + 0);
            //2 triangle ccw
            m_indices.push_back(i + 0);
            m_indices.push_back(i + 1);
            m_indices.push_back(i + m_patchSize + 1);

            i++;
        }
        i++;
    }
    m_indexCount = m_indices.size();
}