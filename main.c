#include "SDL.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        fprintf(stderr, "\nUnable to initialize SDL:  %s\n", SDL_GetError());
        return 1;
    }
    atexit(SDL_Quit);

    return 0;
}
