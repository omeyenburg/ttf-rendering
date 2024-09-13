#include "../ttf.h"

void parse_loca(uint32_t* glyf_offsets,
                uint32_t* glyf_lengths,
                unsigned char* buffer,
                Table* loca,
                uint16_t numGlyphs,
                int16_t indexToLocFormat) {
    if (!loca->initialized) {
        fprintf(stderr, "Table \"loca\" was not found!\n");
        exit(1);
    }

    int offset;
    int last_offset;
    for (int i = 0; i < numGlyphs; i++) {
        if (indexToLocFormat == 0) {
            // Short version
            offset = ((uint32_t) getInt16(buffer, loca->offset + i * 2)) * 2;
        } else {
            // Long version
            offset = getInt32(buffer, loca->offset + i * 4);
        }

        glyf_offsets[i] = offset;
        if (i > 0) {
            glyf_lengths[i - 1] = offset - last_offset;
        }
        last_offset = offset;
        printf("%i: %i\t", i, offset);
    }
}
