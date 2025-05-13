#ifndef MAXP_H
#define MAXP_H

#include "font/font.h"

uint16_t parse_maxp(const unsigned char* restrict buffer, const Table* restrict maxp);

#endif // MAXP_H
