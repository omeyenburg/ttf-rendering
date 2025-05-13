#include "font/tables/loca.h"
#include <stdio.h>
#include <stdlib.h>

void parse_loca(uint32_t* restrict glyf_offsets,
                const unsigned char* restrict buffer,
                const Table* restrict loca,
                const uint16_t numGlyphs,
                const int16_t indexToLocFormat) {
    if (!loca->initialized) {
        fprintf(stderr, "Table \"loca\" was not found!\n");
        exit(1);
    }

    if (!validateCheckSum(buffer, loca, 0)) {
        fprintf(stderr, "Invalid check sum for table \"loca\"!\n");
        exit(1);
    }

    // Populate glyf offsets
    for (uint16_t i = 0; i < numGlyphs; i++) {
        uint32_t offset;
        if (indexToLocFormat == 0) {
            // Short offset (convert to long offset)
            offset = ((uint32_t) getUInt16(buffer, loca->offset + i * 2)) * 2;
        } else {
            // Long offset
            offset = getUInt32(buffer, loca->offset + i * 4);
        }

        glyf_offsets[i] = offset;
    }
}
