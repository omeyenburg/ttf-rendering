#ifndef HHEA_H
#define HHEA_H

#include "font/font.h"

uint16_t parse_hhea(const unsigned char* restrict buffer, const Table* restrict hhea);

#endif // HHEA_H
