#ifndef CMAP_H
#define CMAP_H

#include "font/font.h"
#include "types.h"

uint16_t get_cmap_size(const unsigned char* restrict buffer, const Table* restrict cmap);
void parse_cmap(CharacterMap* restrict charMap,
                const unsigned char* restrict buffer,
                const Table* restrict cmap,
                const uint16_t numChars);

#endif // CMAP_H
