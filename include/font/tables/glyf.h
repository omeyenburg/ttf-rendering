#ifndef GLYPH_H
#define GLYPH_H

#include "font/font.h"
#include "types.h"

void parse_glyf(Glyph* restrict glyphs,
                const unsigned char* restrict buffer,
                const Table* restrict glyf,
                const uint32_t* restrict glyf_offsets,
                const uint16_t numGlyphs);

#endif // GLYPH_H
