#include "font/tables/maxp.h"
#include <stdio.h>
#include <stdlib.h>

uint16_t parse_maxp(unsigned char* buffer, Table* maxp) {
    if (!maxp->initialized) {
        fprintf(stderr, "Table \"maxp\" was not found!\n");
        exit(1);
    }

    if (!validateCheckSum(buffer, maxp, 0)) {
        fprintf(stderr, "Invalid check sum for table \"maxp\"!\n");
        exit(1);
    }

    // Check version
    uint32_t version = getUInt32(buffer, maxp->offset);
    if (version != 0x00010000 && version != 0x00005000) {
        fprintf(stderr, "Unexpected version in maxp table: 0x%08x\n", version);
        exit(1);
    }

    // Read number of glyphs
    uint16_t numGlyphs = getUInt16(buffer, maxp->offset + 4);
    return numGlyphs;
}
