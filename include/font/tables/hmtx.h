#ifndef HMTX_H
#define HMTX_H

#include "font/font.h"

void parse_hmtx(uint16_t* restrict advanceWidth,
                int16_t* restrict leftSideBearings,
                const unsigned char* restrict buffer,
                const Table* restrict hmtx,
                const uint16_t numberOfHMetrics,
                const uint16_t numGlyphs);

#endif // HMTX_H
