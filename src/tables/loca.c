#include "../ttf.h"

void parse_loca(uint32_t* glyf_offsets,
                unsigned char* buffer,
                Table* loca,
                uint16_t numGlyphs,
                int16_t indexToLocFormat) {
    if (!loca->initialized) {
        fprintf(stderr, "Table \"loca\" was not found!\n");
        exit(1);
    }

    if (!validateCheckSum(buffer, loca, 0)) {
        fprintf(stderr, "Invalid check sum for table \"loca\"!\n");
        exit(1);
    }

    // Populate glyf offsets
    for (int i = 0; i < numGlyphs; i++) {
        int offset;
        if (indexToLocFormat == 0) {
            // Short offset (convert to long offset)
            offset = ((uint32_t) getInt16(buffer, loca->offset + i * 2)) * 2;
        } else {
            // Long offset
            offset = getInt32(buffer, loca->offset + i * 4);
        }

        glyf_offsets[i] = offset;
    }
}
