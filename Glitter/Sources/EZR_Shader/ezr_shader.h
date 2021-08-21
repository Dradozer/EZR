#ifndef GLITTER_EZR_SHADER_H
#define GLITTER_EZR_SHADER_H

#define FAIL_PROGRAM {system("pause");  exit(EXIT_FAILURE);}

#include <cstdlib>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GLFW/glfw3.h>

namespace ezr
{
    class Shader
    {
    public:
        static void attachShader(GLuint &program, GLenum type, const char *path);

        static void linkShader(GLuint program);

        static void deleteShader(GLuint program);

    private:
        static GLuint loadShader(GLenum type, const char *path);

        static void validateShader(GLuint shader, const char *path = "");

        static void validateProgram(GLuint program);
    };
}

#endif //GLITTER_EZR_SHADER_H
