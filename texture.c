#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>

GLuint loadTextureJPG(const char* filename)
{
    int w, h, n;
    unsigned char* data = stbi_load(filename, &w, &h, &n, 3);

    if (!data) {
        printf("Erro carregando textura: %s\n", filename);
        return 0;
    }

    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_2D, texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);

    stbi_image_free(data);
    return texID;
}
