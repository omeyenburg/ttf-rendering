#include "ttf.h"
#include "window.h"
#include <stdio.h>

int main(void) {
    // load("resources/ComicMono.ttf");
    load("resources/Ubuntu.ttf");
    Window window = create_window();
    run(&window);
    quit(&window);

    return 0;
}
