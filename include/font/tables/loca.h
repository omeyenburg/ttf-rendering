#ifndef LOCA_H
#define LOCA_H

#include "font/font.h"

void parse_loca(uint32_t* glyf_offsets,
                unsigned char* buffer,
                Table* loca,
                uint16_t numGlyphs,
                int16_t indexToLocFormat);

#endif // LOCA_H
