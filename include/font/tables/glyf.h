#ifndef GLYPH_H
#define GLYPH_H

#include "font/font.h"
#include "types.h"

void parse_simple_glyph(Glyph* glyph,
                        unsigned char* buffer,
                        uint32_t offset,
                        int16_t numberOfContours);

void parse_compound_glyph(Glyph* glyph,
                          unsigned char* buffer,
                          Table* glyf,
                          uint32_t offset,
                          uint32_t* glyf_offsets,
                          uint16_t numGlyphs);

Glyph parse_single_glyph(unsigned char* buffer,
                         Table* glyf,
                         uint32_t* glyf_offsets,
                         uint16_t numGlyphs,
                         uint16_t index);

void parse_glyf(Glyph* glyphs,
                unsigned char* buffer,
                Table* glyf,
                uint32_t* glyf_offsets,
                uint16_t numGlyphs);

#endif // GLYPH_H
