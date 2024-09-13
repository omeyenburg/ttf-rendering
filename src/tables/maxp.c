#include "../ttf.h"

uint16_t parse_maxp(unsigned char* buffer, Table* maxp) {
    if (!maxp->initialized) {
        fprintf(stderr, "Table \"maxp\" was not found!\n");
        exit(1);
    }

    uint16_t numGlyphs = getInt16(buffer, maxp->offset + 4);
    return numGlyphs;
}
