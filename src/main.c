#include "font/ttf.h"
#include "graphics/window.h"

int main() {
    load_ttf("resources/fonts/ubuntu/Ubuntu-Bold.ttf");
    load_ttf("resources/fonts/courier-new/courier-new.ttf");
    const Window window = create_window();
    run(&window);
    quit(&window);
}
