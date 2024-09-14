#include "window.h"
#include "SDL.h"
#include <GL/gl.h>
#include <GL/glu.h>

int create_window() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
        return 1;
    }

    // Set OpenGL version to 3.3 core
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1); // prob. needed ( TODO:)

    // TODO: needed?
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 5);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);

    // Create SDL window
    SDL_Window* window = SDL_CreateWindow("Font Renderer",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          800,
                                          600,
                                          SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    if (window == NULL) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create an OpenGL context
    SDL_GLContext glContext = SDL_GL_CreateContext(window);
    if (glContext == NULL) {
        printf("OpenGL context could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Get newest supported OpenGL version
    const GLubyte* version = glGetString(GL_VERSION);
    printf("Newest supported OpenGL version: %s\n", version);

    // Initialize OpenGL
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // Set clear color
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    // glEnable(GL_DEPTH_TEST);              // Enable depth testing TODO: needed?

    // Main loop flag
    int running = 1;
    SDL_Event event;

    // Main loop
    while (running) {
        // Handle events
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = 0; // Exit the loop on quit event
            }
        }

        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Swap the window buffers
        SDL_GL_SwapWindow(window);
    }

    // Cleanup TODO: separate function
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
