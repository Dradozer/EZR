#ifndef GLITTER_EZR_SSBO_H
#define GLITTER_EZR_SSBO_H

#include "ezr_shader.h"

namespace ezr
{
    class SSBO
    {
    public:
        template<typename T>
        static void createSSBO(GLuint &id, int bindingID, int size, T *data)
        {
            glCreateBuffers(1, &id);
            glBindBuffer(GL_SHADER_STORAGE_BUFFER, id);
            glBindBufferBase(GL_SHADER_STORAGE_BUFFER, bindingID, id);
            glBufferData(GL_SHADER_STORAGE_BUFFER, size, data, GL_DYNAMIC_DRAW);
        }
    };
}

#endif //GLITTER_EZR_SSBO_H
