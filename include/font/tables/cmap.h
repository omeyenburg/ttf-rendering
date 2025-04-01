#ifndef CMAP_H
#define CMAP_H

#include "font/font.h"
#include "types.h"

uint16_t get_cmap_size(const unsigned char* buffer, const Table* cmap);
void parse_cmap(CharacterMap* charMap,
                const unsigned char* buffer,
                const Table* cmap,
                const uint16_t numChars);

#endif // CMAP_H
