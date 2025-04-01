#ifndef GLYPH_H
#define GLYPH_H

#include "font/font.h"
#include "types.h"

void parse_glyf(Glyph* glyphs,
                const unsigned char* buffer,
                const Table* glyf,
                const uint32_t* glyf_offsets,
                const uint16_t numGlyphs);

#endif // GLYPH_H
