#include "../ttf.h"

void parse_hmtx(uint16_t* advanceWidth,
                int16_t* leftSideBearings,
                unsigned char* buffer,
                Table* hmtx,
                uint16_t numberOfHMetrics,
                uint16_t numGlyphs) {
    if (!hmtx->initialized) {
        fprintf(stderr, "Table \"hmtx\" was not found!\n");
        exit(1);
    }

    if (!validateCheckSum(buffer, hmtx, 0)) {
        fprintf(stderr, "Invalid check sum for table \"hmtx\"!\n");
        exit(1);
    }

    uint32_t offset = hmtx->offset;
    for (int i = 0; i < numberOfHMetrics; i++) {
        advanceWidth[i] = getUInt16(buffer, offset);
        leftSideBearings[i] = getInt16(buffer, offset + 2);
        offset += 4;
    }

    for (int i = numberOfHMetrics; i < numGlyphs; i++) {
        leftSideBearings[i] = getInt16(buffer, offset);
        offset += 2;
    }
}
