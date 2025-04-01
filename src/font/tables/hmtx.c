#include "font/tables/hmtx.h"
#include <stdio.h>
#include <stdlib.h>

void parse_hmtx(uint16_t* advanceWidth,
                int16_t* leftSideBearings,
                const unsigned char* buffer,
                const Table* hmtx,
                const uint16_t numberOfHMetrics,
                const uint16_t numGlyphs) {
    if (!hmtx->initialized) {
        fprintf(stderr, "Table \"hmtx\" was not found!\n");
        exit(1);
    }

    if (!validateCheckSum(buffer, hmtx, 0)) {
        fprintf(stderr, "Invalid check sum for table \"hmtx\"!\n");
        exit(1);
    }

    uint32_t offset = hmtx->offset;
    for (uint16_t i = 0; i < numberOfHMetrics; i++) {
        advanceWidth[i] = getUInt16(buffer, offset);
        leftSideBearings[i] = getInt16(buffer, offset + 2);
        offset += 4;
    }

    for (uint16_t i = numberOfHMetrics; i < numGlyphs; i++) {
        leftSideBearings[i] = getInt16(buffer, offset);
        offset += 2;
    }
}
