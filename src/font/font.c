#include "font/font.h"
#include <stdio.h>
#include <stdlib.h>

uint16_t getUInt16(const uint8_t* buffer, const size_t offset) {
    return ((uint16_t) buffer[offset] << 8) | (uint16_t) buffer[offset + 1];
}

int16_t getInt16(const uint8_t* buffer, const size_t offset) {
    return (int16_t) (((uint16_t) buffer[offset] << 8) | buffer[offset + 1]);
}

uint32_t getUInt24(const uint8_t* buffer, const size_t offset) {
    return ((uint32_t) buffer[offset] << 16) | ((uint32_t) buffer[offset + 1] << 8) |
           (uint32_t) buffer[offset + 2];
}

uint32_t getUInt32(const uint8_t* buffer, const size_t offset) {
    return ((uint32_t) buffer[offset] << 24) | ((uint32_t) buffer[offset + 1] << 16) |
           ((uint32_t) buffer[offset + 2] << 8) | (uint32_t) buffer[offset + 3];
}

char* bytes4xchar(const unsigned char* buffer, const int offset) {
    static char string[5];
    string[0] = buffer[offset];
    string[1] = buffer[offset + 1];
    string[2] = buffer[offset + 2];
    string[3] = buffer[offset + 3];
    string[4] = '\0';

    return string;
}

uint16_t unicode(const unsigned char* c) {
    if ((c[0] >> 7) == 0) {
        // 1-byte character (ASCII)
        return c[0];
    } else if ((c[0] >> 5) == 0b110) {
        // 2-byte character
        return ((c[0] & 0x1F) << 6) | (c[1] & 0x3F);
    } else {
        fprintf(stderr,
                "Unsupported unicode character size. Only 1 and 2 bytes allowed.\n");
        exit(1);
    }
}

bool validateCheckSum(const unsigned char* restrict buffer,
                      const Table* restrict table,
                      const uint32_t adjustment) {
    const uint32_t full_parts = table->length / 4;
    const uint32_t remainder = table->length - full_parts * 4;

    uint32_t sum = 0;
    for (size_t i = 0; i < full_parts; i++) {
        sum += getUInt32(buffer, table->offset + 4 * i);
    }

    switch (remainder) {
        case 1:
            sum += buffer[table->offset + 4 * full_parts] << 24;
            break;
        case 2:
            sum += getUInt16(buffer, table->offset + 4 * full_parts) << 16;
            break;
        case 3:
            sum += getUInt24(buffer, table->offset + 4 * full_parts) << 8;
            break;
    }

    // Necessary for head table
    sum -= adjustment;

    return sum == table->checkSum;
}
