#ifndef GLYF_H
#define GLYF_H
#include "../ttf.h"

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
                         uint16_t numGlyphs);

#endif // GLYF_H
