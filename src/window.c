#include "window.h"

#define nullptr (void*) 0

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

    // Enable vsync
    SDL_GL_SetSwapInterval(1);

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

    // Open file
    FILE* fp;
    fp = fopen("resources/shader/vertex.glsl", "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file.\n");
        exit(1);
    }

    // Get file size
    fseek(fp, 0L, SEEK_END);
    int size = ftell(fp);
    fseek(fp, 0L, SEEK_SET); // or use rewind(fp);

    // Allocate memory for the shader code
    char* vertex_code =
        (char*) malloc((size + 1) * sizeof(char)); // +1 for null terminator
    if (vertex_code == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    // Read the file contents into the allocated memory
    size_t read_size = fread(vertex_code, sizeof(char), size, fp);
    vertex_code[read_size] = '\0'; // Null-terminate the string

    // Open file
    fp = fopen("resources/shader/fragment.glsl", "r");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file.\n");
        exit(1);
    }

    // Get file size
    fseek(fp, 0L, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0L, SEEK_SET); // or use rewind(fp);

    // Allocate memory for the shader code
    char* fragment_code =
        (char*) malloc((size + 1) * sizeof(char)); // +1 for null terminator
    if (fragment_code == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(1);
    }

    // Read the file contents into the allocated memory
    read_size = fread(fragment_code, sizeof(char), size, fp);
    fragment_code[read_size] = '\0'; // Null-terminate the string

    // Create a shader program
    GLint gl_status;
    GLuint program = glCreateProgram();

    // Create a vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_code, nullptr);
    glCompileShader(vertex_shader);
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &gl_status);
    if (gl_status != GL_TRUE) {
        GLint log_length = 0;
        glGetShaderiv(vertex_shader, GL_INFO_LOG_LENGTH, &log_length);
        GLchar* message = (GLchar*) malloc(log_length);
        glGetShaderInfoLog(vertex_shader, log_length, nullptr, message);
        fprintf(stderr, "Vertex shader compilation failed:\n%s\n", message);
        free(message);
        exit(1);
    }
    glAttachShader(program, vertex_shader);

    // Create a fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_code, nullptr);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &gl_status);
    if (gl_status != GL_TRUE) {
        GLint log_length = 0;
        glGetShaderiv(fragment_shader, GL_INFO_LOG_LENGTH, &log_length);
        GLchar* message = (GLchar*) malloc(log_length);
        glGetShaderInfoLog(fragment_shader, log_length, nullptr, message);
        fprintf(stderr, "Fragment shader compilation failed:\n%s\n", message);
        free(message);
        exit(1);
    }
    glAttachShader(program, fragment_shader);

    // Link the program
    glLinkProgram(program);
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

    // VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // VBO
    GLuint vbo;
    glGenBuffers(1, &vbo);

    // EBO
    GLuint ebo;
    glGenBuffers(1, &ebo);

    // Vertices
    GLfloat vertices[] = {-1, -1, -1, 1, 1, 1, 1, -1};
    GLuint indices[] = {0, 1, 2, 0, 2, 3};

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(GLfloat), vertices, GL_STATIC_DRAW);

    // Vertices
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);

    // Indices
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);

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

        glBindVertexArray(vao);
        glUseProgram(program);
        // Clear the screen and depth buffer
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

        // Swap the window buffers
        SDL_GL_SwapWindow(window);
    }

    // Cleanup TODO: separate function
    SDL_GL_DeleteContext(glContext);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
