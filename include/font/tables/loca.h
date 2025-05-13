#ifndef LOCA_H
#define LOCA_H

#include "font/font.h"

void parse_loca(uint32_t* restrict glyf_offsets,
                const unsigned char* restrict buffer,
                const Table* restrict loca,
                const uint16_t numGlyphs,
                const int16_t indexToLocFormat);

#endif // LOCA_H
