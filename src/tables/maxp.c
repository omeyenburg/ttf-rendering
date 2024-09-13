#include "../ttf.h"

uint16_t parse_maxp(unsigned char* buffer, Table* maxp) {
    if (!maxp->initialized) {
        fprintf(stderr, "Table \"maxp\" was not found!\n");
        exit(1);
    }

    uint32_t version = getInt32(buffer, maxp->offset);

    // Allow both 0x00010000 and 0x00005000 versions
    if (version != 0x00010000 && version != 0x00005000) {
        fprintf(stderr, "Unexpected version in maxp table: 0x%08x\n", version);
        exit(1);
    }

    uint16_t numGlyphs = getInt16(buffer, maxp->offset + 4);
    return numGlyphs;
}
