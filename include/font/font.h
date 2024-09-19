#ifndef FONT_H
#define FONT_H

#include <stdbool.h>
#include <stdint.h>

#define getUInt16(buffer, offset)                                                      \
    (((unsigned char) buffer[offset] << 8) | ((unsigned char) buffer[offset + 1]))
#define getInt16(buffer, offset)                                                       \
    (((char) buffer[offset] << 8) | ((char) buffer[offset + 1]))
#define getUInt24(buffer, offset)                                                      \
    ((unsigned char) buffer[offset] << 16) |                                           \
        ((unsigned char) buffer[offset + 1] << 8) |                                    \
        ((unsigned char) buffer[offset + 2])
#define getUInt32(buffer, offset)                                                      \
    (((unsigned char) buffer[offset] << 24) |                                          \
     ((unsigned char) buffer[offset + 1] << 16) |                                      \
     ((unsigned char) buffer[offset + 2] << 8) | ((unsigned char) buffer[offset + 3]))

typedef struct {
    bool initialized;
    uint32_t checkSum;
    uint32_t offset;
    uint32_t length;
} Table;

char* bytes4xchar(unsigned char* buffer, int offset);
uint16_t unicode(unsigned char* c);
bool validateCheckSum(unsigned char* buffer, Table* table, uint32_t adjustment);

#endif // FONT_H
