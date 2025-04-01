#ifndef HHEA_H
#define HHEA_H

#include "font/font.h"

uint16_t parse_hhea(const unsigned char* buffer, const Table* hhea);

#endif // HHEA_H
