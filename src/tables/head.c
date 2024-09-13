#include "../ttf.h"

int16_t parse_head(unsigned char* buffer, Table* head) {
    if (!head->initialized) {
        fprintf(stderr, "Table \"head\" was not found!:\n");
        exit(1);
    }

    int16_t indexToLocFormat = getInt16(buffer, head->offset + 50);
    return indexToLocFormat;
}
