#include "graphics/window.h"

int main() {
    Window window = create_window();
    run(&window);
    quit(&window);
}
