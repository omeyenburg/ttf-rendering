#ifndef HMTX_H
#define HMTX_H

#include "font/font.h"

void parse_hmtx(uint16_t* advanceWidth,
                int16_t* leftSideBearings,
                unsigned char* buffer,
                Table* hmtx,
                uint16_t numberOfHMetrics,
                uint16_t numGlyphs);

#endif // HMTX_H
