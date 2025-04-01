#ifndef FONT_H
#define FONT_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h> // provide size_t

/*uint16_t m_getUint16(unsigned char* buffer, size_t offset) {*/
/*    return ((unsigned char) buffer[offset] << 8) | ((unsigned char) buffer[offset + 1]);*/
/*}*/

/*#define getUInt16(buffer, offset)                                                      \*/
/*    (((unsigned char) buffer[offset] << 8) | ((unsigned char) buffer[offset + 1]))*/
/*#define getInt16(buffer, offset)                                                       \*/
/*    (((char) buffer[offset] << 8) | ((char) buffer[offset + 1]))*/
/*#define getUInt24(buffer, offset)                                                      \*/
/*    ((unsigned char) buffer[offset] << 16) |                                           \*/
/*        ((unsigned char) buffer[offset + 1] << 8) |                                    \*/
/*        ((unsigned char) buffer[offset + 2])*/
/*#define getUInt32(buffer, offset)                                                      \*/
/*    (((unsigned char) buffer[offset] << 24) |                                          \*/
/*     ((unsigned char) buffer[offset + 1] << 16) |                                      \*/
/*     ((unsigned char) buffer[offset + 2] << 8) | ((unsigned char) buffer[offset + 3]))*/

typedef struct {
    bool initialized;
    uint32_t checkSum;
    uint32_t offset;
    uint32_t length;
} Table;

uint16_t getUInt16(const uint8_t* buffer, const size_t offset);
int16_t getInt16(const uint8_t* buffer, const size_t offset);
uint32_t getUInt24(const uint8_t* buffer, const size_t offset);
uint32_t getUInt32(const uint8_t* buffer, const size_t offset);
char* bytes4xchar(const unsigned char* buffer, const int offset);
uint16_t unicode(const unsigned char* c);
bool validateCheckSum(const unsigned char* buffer, const Table* table, const uint32_t adjustment);

#endif // FONT_H
