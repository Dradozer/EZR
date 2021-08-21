
#include "ezr_shader.h"

void ezr::Shader::attachShader(GLuint &program, GLenum type, const char *path)
{
    GLuint shader = loadShader(type, path);

    if (glIsProgram(program) == false)
    {
        program = glCreateProgram();
    }
    glAttachShader(program, shader);
}

void ezr::Shader::linkShader(GLuint program)
{
    glLinkProgram(program);
    validateProgram(program);
    glValidateProgram(program);
    validateProgram(program);
}

GLuint ezr::Shader::loadShader(GLenum type, const char *path)
{
    std::string fileContent;
    std::string line;

    std::ifstream file(path);
    if (file.is_open())
    {
        while (!file.eof())
        {
            getline(file, line);
            fileContent += line + "\n";
        }
        file.close();
        std::cout << "Successfully loaded:\t\t" << path << std::endl;
    } else
    {
        std::cout << "ERROR:\t\tLoading-Error with " << path << std::endl;
    }
    const char *source = fileContent.c_str();
    const GLint sourceSize = strlen(source);
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &source, &sourceSize);
    glCompileShader(shader);
    validateShader(shader, path);

    return shader;
}

void ezr::Shader::validateShader(GLuint shader, const char *path)
{
    GLint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);

    if (status == GL_FALSE)
    {
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        GLchar *strLog = new GLchar[logLength + 1];
        glGetShaderInfoLog(shader, logLength, NULL, strLog);

        std::cout  << "ERROR:\t\tCompile-Error with " << path << std::endl;
        std::cout  << strLog  << std::endl;

        delete[] strLog;

    } else if (status == GL_TRUE)
    {
        std::cout  << "Successfully compiled:\t\t" << path << std::endl;
    }
}

void ezr::Shader::validateProgram(GLuint program)
{
    GLint status = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &status);

    if (status == GL_FALSE)
    {
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        GLchar *strLog = new GLchar[logLength + 1];
        glGetProgramInfoLog(program, logLength, NULL, strLog);

        std::cout << "ERROR:\t\tLinking-Error" << std::endl;
        std::cout << strLog << std::endl;

        delete[] strLog;

        deleteShader(program);

        FAIL_PROGRAM //program shuts down
    }
    GLenum glError;
    if ((glError = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "Shader validation OpenGL-Error: " << glError << std::endl;
    }
}

void ezr::Shader::deleteShader(GLuint program)
{
    GLint shaderCount = 0;
    glGetProgramiv(program, GL_ATTACHED_SHADERS, &shaderCount);
    if (shaderCount != 0)
    {
        GLuint *shader = new GLuint[shaderCount];
        glGetAttachedShaders(program, shaderCount, NULL, shader);
        for(int i = 0; i < shaderCount; i++)
        {
            glDeleteShader(shader[i]);
        }
        delete [] shader;
        glDeleteProgram(program);
    }
    GLenum glError;
    if ((glError = glGetError()) != GL_NO_ERROR)
    {
        std::cout << "Shader delete OpenGL-Error: " << glError << std::endl;
    }
}