#ifndef TYPES_H
#define TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define float16 _Float16

typedef struct {
    uint16_t unicode;
    uint16_t index;
} CharacterMap;

typedef struct {
    int16_t x;
    int16_t y;
    bool onCurve;
} Point;

typedef struct {
    int16_t numberOfContours;
    int16_t xMin;
    int16_t yMin;
    int16_t xMax;
    int16_t yMax;
    uint32_t numPoints;
    Point* points;
    uint16_t* endPtsOfContours;
} Glyph;

#endif // TYPES_H
