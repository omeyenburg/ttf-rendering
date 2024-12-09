#include "graphics/window.h"
#include "graphics/buffer.h"
#include "graphics/graphics.h"
#include "graphics/shader.h"
#include "graphics/texture.h"
#include <GL/gl.h>
#include <stdio.h>
#include <stdlib.h>

Window create_window() {
    Window window;

    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "Unable to initialize SDL: %s\n", SDL_GetError());
        exit(1);
    }

    // Set OpenGL version to 3.3 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    // Create SDL window
    window.window = SDL_CreateWindow("Font Renderer",
                                     SDL_WINDOWPOS_CENTERED,
                                     SDL_WINDOWPOS_CENTERED,
                                     800,
                                     600,
                                     SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window.window == NULL) {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    // Enable vsync
    SDL_GL_SetSwapInterval(1);

    // Create an OpenGL context
    window.gl_context = SDL_GL_CreateContext(window.window);
    if (window.gl_context == NULL) {
        fprintf(stderr,
                "OpenGL context could not be created! SDL_Error: %s\n",
                SDL_GetError());
        SDL_DestroyWindow(window.window);
        SDL_Quit();
        exit(1);
    }

    // Initialize OpenGL
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set clear color
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_DEPTH_TEST);              // Enable depth testing TODO: needed?

    // Create shader program
    const char* vertex = "resources/shader/vertex.glsl";
    const char* fragment = "resources/shader/fragment.glsl";
    window.shader_program = create_shader(vertex, fragment);

    // Create buffers
    window.vao = create_buffers();

    return window;
}

void run(Window* window) {
    int running = 1;
    SDL_Event event;

    // Main loop TODO: separate function
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0;
            }
        }

        // Apply shader and vao
        glBindVertexArray(window->vao);
        glUseProgram(window->shader_program);

        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Draw using shader
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Swap the window buffers
        SDL_GL_SwapWindow(window->window);
    }
}

void quit(Window* window) {
    // Cleanup
    glDeleteProgram(window->shader_program);
    SDL_GL_DeleteContext(window->gl_context);
    SDL_DestroyWindow(window->window);
    SDL_Quit();
}
