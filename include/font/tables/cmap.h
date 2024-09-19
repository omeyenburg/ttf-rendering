#ifndef CMAP_H
#define CMAP_H

#include "font/font.h"
#include "types.h"

uint16_t get_cmap_size(unsigned char* buffer, Table* cmap);
void parse_cmap(CharacterMap* charMap,
                unsigned char* buffer,
                Table* cmap,
                uint16_t numChars);

#endif // CMAP_H
