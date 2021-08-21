#ifndef GLITTER_EZR_COMPUTESHADER_H
#define GLITTER_EZR_COMPUTESHADER_H

#include "ezr_shader.h"

namespace ezr
{
    class ComputeShader
    {
    public:
        static void createComputeShader(GLuint &id, const char *path)
        {
            Shader::attachShader(id, GL_COMPUTE_SHADER, path);
            Shader::linkShader(id);
        }

        static void initComputeShader(GLuint *id) //add variable
        {
            GLenum glError;
            if ((glError = glGetError()) != GL_NO_ERROR)
            {
                std::cout << "CompShader init 1.OpenGL-Error: " << glError << std::endl;
            }
            glUseProgram(id[0]);
            {
//                glUniform1ui(id[2], variable);
            }
            glUseProgram(0);
            if ((glError = glGetError()) != GL_NO_ERROR)
            {
                std::cout << "CompShader init 2.OpenGL-Error: " << glError << std::endl;
            }
        }

        static void updateComputeShader(GLuint *id)
        {
            int dispatch = 10;
            GLenum glError;
            if ((glError = glGetError()) != GL_NO_ERROR)
            {
                std::cout << "CompShader update 1.OpenGL-Error: " << glError << std::endl;
            }
            glUseProgram(id[0]);
            {
                glDispatchCompute(ceil(dispatch), 1, 1); // shader layout
                glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            }
            glUseProgram(0);
            if ((glError = glGetError()) != GL_NO_ERROR)
            {
                std::cout << "CompShader update 2.OpenGL-Error: " << glError << std::endl;
            }
        }

    };
}

#endif //GLITTER_EZR_COMPUTESHADER_H
