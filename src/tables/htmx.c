#include "../ttf.h"

void parse_htmx(unsigned char* buffer, Table* htmx) {
    if (!htmx->initialized) {
        fprintf(stderr, "Table \"htmx\" was not found!:\n");
        exit(1);
    }
}
