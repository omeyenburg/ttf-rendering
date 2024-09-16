#include "window.h"

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

GLuint attach_shader(GLuint program, GLenum type, const char* path) {
    const char* code = read_file(path);

    GLuint shader = glCreateShader(type);
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

GLuint create_buffers() {
    // VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Values for vertices and indices
    GLfloat vertices[] = {-1, -1, -1, 1, 1, 1, 1, -1};
    GLuint indices[] = {0, 1, 2, 0, 2, 3};

    // VBO & EBO
    GLuint vbo, ebo;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);

    // Vertices
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
    return vao;
}

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
