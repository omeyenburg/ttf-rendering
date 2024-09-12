#ifndef TTF_H
#define TTF_H
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

struct Table {
    int initialized;
    long check_sum;
    long offset;
    long length;
};

void load(char* path);

#endif
