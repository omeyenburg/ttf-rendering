#include "graphics/texture.h"
#include "graphics/graphics.h"
#include <GL/gl.h>

void create_textures() {
    GLuint tex_points;
    glGenTextures(1, &tex_points);
    glBindTexture(GL_TEXTURE_1D, tex_points);
    glActiveTexture(GL_TEXTURE0);

    GLsizei width = 0;

    // These floats are 2 bytes in size:
    // _Float16 test;

    glTexImage1D(
        GL_TEXTURE_1D, 0, GL_RG16F, width, 0, GL_RG, GL_UNSIGNED_BYTE, nullptr);
    // or GL_RG16I/GL_RG16UI & GL_RG_INTEGER

    // Disable interpolation
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}
