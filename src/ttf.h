#ifndef TTF_H
#define TTF_H

// #include "tables/glyf.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int32_t Fixed;     // 16.16 signed fixed-point
typedef int16_t F2Dot14;   // 2.14 signed fixed-point
typedef int16_t FWord;     // Signed 16-bit integer
typedef uint16_t uFWord;   // Unsigned 16-bit integer
typedef int16_t shortFrac; // 0.16 signed fixed-point

typedef struct {
    int initialized;
    long checkSum;
    long offset;
    long length;
} Table;

typedef struct {
    int16_t x;
    int16_t y;
    bool onCurve;
} Point;

typedef struct {
    int16_t numberOfContours;
    FWord xMin;
    FWord yMin;
    FWord xMax;
    FWord yMax;
    uint32_t numPoints;
    Point* points;
    uint16_t* endPtsOfContours;
} Glyph;

#define getInt16(buffer, offset)                                                       \
    (((unsigned char) buffer[offset] << 8) | ((unsigned char) buffer[offset + 1]))
#define getInt32(buffer, offset)                                                       \
    (((unsigned char) buffer[offset] << 24) |                                          \
     ((unsigned char) buffer[offset + 1] << 16) |                                      \
     ((unsigned char) buffer[offset + 2] << 8) | ((unsigned char) buffer[offset + 3]))

void load(char* path);
int16_t parse_head(unsigned char* buffer, Table* head);
uint16_t parse_maxp(unsigned char* buffer, Table* maxp);
void parse_loca(uint32_t* glyf_offsets,
                uint32_t* glyf_lengths,
                unsigned char* buffer,
                Table* loca,
                uint16_t numGlyphs,
                int16_t indexToLocFormat);
void parse_glyf(unsigned char* buffer,
                Table* glyf,
                uint32_t* glyf_offsets,
                uint32_t* glyf_lengths,
                uint16_t numGlyphs);
void parse_cmap(unsigned char* buffer, Table* cmap);
void parse_htmx(unsigned char* buffer, Table* htmx);

#endif // TTF_H
