#include "ttf.h"
#include "window.h"
#include <stdio.h>

int main(void) {
    printf("Running\n");
    load("resources/ComicMono.ttf");
    create_window();

    return 0;
}
