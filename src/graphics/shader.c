#include "graphics/shader.h"
#include "graphics/graphics.h"
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>

const char* read_file(const char* path) {
    // Open file
    FILE* fp;
    fp = fopen(path, "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file %s.\n", path);
        exit(1);
    }

    // Get file size
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0L, SEEK_SET);

    // Allocate memory for the shader code (+1 for null terminator)
    char* content = (char*) malloc((size + 1) * sizeof(char));
    if (content == NULL) {
        fprintf(stderr, "Memory allocation failed while reading file %s.\n", path);
        exit(1);
    }

    // Read the file contents into the allocated memory
    fread(content, sizeof(char), size, fp);

    // Null-terminate the string
    content[size] = '\0';

    return content;
}

GLuint attach_shader(GLuint program, GLenum shader_type, const char* path) {
    const char* code = read_file(path);

    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &code, nullptr);
    glCompileShader(shader);

    GLint gl_status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &gl_status);
    if (gl_status != GL_TRUE) {
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        GLchar* message = (GLchar*) malloc(log_length);
        glGetShaderInfoLog(shader, log_length, nullptr, message);
        fprintf(stderr, "Shader compilation failed:\n%s\n", message);
        free(message);
        exit(1);
    }
    glAttachShader(program, shader);

    return shader;
}

GLuint create_shader(const char* vertex, const char* fragment) {
    GLuint program = glCreateProgram();

    GLuint vertex_shader = attach_shader(program, GL_VERTEX_SHADER, vertex);
    GLuint fragment_shader = attach_shader(program, GL_FRAGMENT_SHADER, fragment);

    // Link the program
    glLinkProgram(program);

    GLint gl_status;
    glGetProgramiv(program, GL_LINK_STATUS, &gl_status);
    if (gl_status == GL_FALSE) {
        GLint log_length = 0;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &log_length);
        GLchar message[log_length];
        glGetProgramInfoLog(program, log_length, &log_length, message);
        glDeleteProgram(program);
        fprintf(stderr, "Shader linking failed:\n%s\n", message);
        exit(1);
    }

    // Delete the shader object
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return program;
}
