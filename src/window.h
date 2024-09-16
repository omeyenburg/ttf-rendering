#ifndef WINDOW_H
#define WINDOW_H
#include <GL/gl.h>
#include <SDL2/SDL.h>
#include <stdio.h>

#define nullptr (void*) 0

typedef struct {
    SDL_GLContext gl_context;
    SDL_Window* window;
    GLuint shader_program;
    GLuint vao;
} Window;

Window create_window();
void run(Window* window);
void quit(Window* window);

#endif
