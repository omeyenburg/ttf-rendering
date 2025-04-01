#ifndef HMTX_H
#define HMTX_H

#include "font/font.h"

void parse_hmtx(uint16_t* advanceWidth,
                int16_t* leftSideBearings,
                const unsigned char* buffer,
                const Table* hmtx,
                const uint16_t numberOfHMetrics,
                const uint16_t numGlyphs);

#endif // HMTX_H
