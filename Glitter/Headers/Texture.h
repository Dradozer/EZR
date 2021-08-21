//
// Created by mhun on 26.05.20.
//

#ifndef GLITTER_TEXTURE_H
#define GLITTER_TEXTURE_H

#include "glitter.hpp"
#include <string>
#include <iostream>
#include <glad/glad.h>

class Texture {

public:

    // is3D? is2D? loadTexture?
    Texture();
    ~Texture();

    GLuint getTexture(const char* path_to_texture, int dimension, int channels, GLint internalformat, GLenum format, GLenum type);
    GLuint getTexture(const char* path_to_texture, int dimension, int channels);
    GLuint generate3DTexture(int width, int height, int depth, GLint internalformat, GLenum format, GLenum type);
    GLuint generate2DTexture(int width, int height, GLint internalformat, GLenum format, GLenum type);
};


#endif //GLITTER_TEXTURE_H
