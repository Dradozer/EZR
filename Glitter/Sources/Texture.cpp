//
// Created by mhun on 26.05.20.
//

#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

Texture::Texture(){
}

unsigned int Texture::getTexture(const char* path_to_texture, int dimension, int channels, GLint internalformat, GLenum format, GLenum type) {
    unsigned int textureID;
    const auto &img = stbi_load(path_to_texture, &dimension, &dimension, &channels, channels);


    if(img != nullptr)
    {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, internalformat, dimension, dimension, 0, format, type, img);//access violation
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        stbi_image_free(img);
        return textureID;
    }
    std::cout << "Texture failed to load at path: " << path_to_texture << std::endl;
    //return textureID;
}

unsigned int Texture::getTexture(const char *path_to_texture, int dimension, int channels) {
    unsigned int textureID;
    const auto &img = stbi_load(path_to_texture, &dimension, &dimension, &channels, 0);


    if (img != nullptr) {
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, dimension, dimension, 0, GL_RGBA, GL_UNSIGNED_BYTE, img);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(img);
        return textureID;
    }
    std::cout << "Texture failed to load at path: " << path_to_texture << std::endl;
    //return textureID;
}

unsigned int Texture::generate2DTexture(int width, int height, GLint internalformat, GLenum format, GLenum type) {
    GLenum glError;
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexImage2D(GL_TEXTURE_2D, 0, internalformat, width, height, 0, format, type, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return textureID;
}

unsigned int
Texture::generate3DTexture(int width, int height, int depth, GLint internalformat, GLenum format, GLenum type) {
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_3D, textureID);
    glTexImage3D(GL_TEXTURE_3D, 0, internalformat, width, height, depth, 0, format, type, NULL);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    return textureID;
}
