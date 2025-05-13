#include "font/tables/cmap.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

uint16_t get_cmap_size(const unsigned char* restrict buffer, const Table* restrict cmap) {
    if (!cmap->initialized) {
        fprintf(stderr, "Table \"cmap\" was not found!\n");
        exit(1);
    }

    const u_int16_t version = getUInt16(buffer, cmap->offset);
    if (version != 0) {
        fprintf(stderr, "Expected CMAP version 0, got %i.", version);
        exit(1);
    }

    const u_int16_t numSubtables = getUInt16(buffer, cmap->offset + 2);
    if (numSubtables == 0) {
        fprintf(stderr, "Could not find any sub tables in CMAP table.");
        exit(1);
    }

    uint16_t platform_id = 4;
    uint32_t offset = 0;
    uint32_t buffer_offset = cmap->offset + 4;
    for (size_t i = 0; i < numSubtables; i++) {
        uint16_t sub_platform_id = getUInt16(buffer, buffer_offset);
        uint32_t sub_offset = getUInt32(buffer, buffer_offset + 4);

        if (platform_id == 4 || platform_id != 0 ||
            (platform_id == 1 && sub_platform_id == 3)) {
            platform_id = sub_platform_id;
            offset = cmap->offset + sub_offset;
        }

        buffer_offset += 8;
    }

    const uint16_t format = getUInt16(buffer, offset);
    if (format != 4) {
        fprintf(stderr,
                "Unsupported format %i. Currently only format 4 is supported.",
                format);
        exit(1);
    }

    const uint16_t segCount = getUInt16(buffer, offset + 6) / 2;
    uint32_t charCount = 0;
    for (size_t i = 0; i < segCount; i++) {
        uint16_t endCode = getUInt16(buffer, offset + 14 + i * 2);
        uint16_t startCode = getUInt16(buffer, offset + 16 + segCount * 2 + i * 2);
        if (startCode <= endCode) {
            // Number of characters in this segment
            charCount += (endCode - startCode + 1);
        }
    }

    return charCount;
}

void parse_cmap(CharacterMap* restrict charMap,
                const unsigned char* restrict buffer,
                const Table* restrict cmap,
                const uint16_t numChars) {
    if (!cmap->initialized) {
        fprintf(stderr, "Table \"cmap\" was not found!\n");
        exit(1);
    }

    if (!validateCheckSum(buffer, cmap, 0)) {
        fprintf(stderr, "Invalid check sum for table \"cmap\"!\n");
        exit(1);
    }

    const u_int16_t version = getUInt16(buffer, cmap->offset);
    if (version != 0) {
        fprintf(stderr, "Expected CMAP version 0, got %i.", version);
        exit(1);
    }

    const u_int16_t numSubtables = getUInt16(buffer, cmap->offset + 2);
    if (numSubtables == 0) {
        fprintf(stderr, "Could not find any sub tables in CMAP table.");
        exit(1);
    }

    uint16_t platform_id = 4;
    uint32_t offset = 0;
    uint32_t buffer_offset = cmap->offset + 4;
    for (size_t i = 0; i < numSubtables; i++) {
        uint16_t sub_platform_id = getUInt16(buffer, buffer_offset);
        uint32_t sub_offset = getUInt32(buffer, buffer_offset + 4);

        if (platform_id == 4 || platform_id != 0 ||
            (platform_id == 1 && sub_platform_id == 3)) {
            platform_id = sub_platform_id;
            offset = cmap->offset + sub_offset;
        }

        buffer_offset += 8;
    }

    const uint16_t format = getUInt16(buffer, offset);
    if (format != 4) {
        fprintf(stderr,
                "Unsupported format %i. Currently only format 4 is supported.",
                format);
        exit(1);
    }

    // Read segment count
    const uint16_t segCount = getUInt16(buffer, offset + 6) / 2;

    // Read reservedPad (2 bytes) - it is always 0
    const uint16_t reservedPad = getUInt16(buffer, offset + 14 + segCount * 2);
    if (reservedPad != 0) {
        fprintf(stderr, "Control value reservedPad should always be 0!");
        exit(1);
    }

    uint16_t mapIndex = 0;
    for (uint16_t i = 0; i < segCount; i++) {
        const uint16_t endCode = getUInt16(buffer, offset + 14 + i * 2);
        const uint16_t startCode = getUInt16(buffer, offset + 16 + segCount * 2 + i * 2);
        const uint16_t idDelta = getUInt16(buffer, offset + 16 + segCount * 4 + i * 2);
        const uint16_t idRangeOffset = getUInt16(buffer, offset + 16 + segCount * 6 + i * 2);
        for (uint32_t character_code = startCode; character_code <= endCode;
             character_code++) {
            uint16_t glyph_index;
            if (idRangeOffset == 0) {
                glyph_index = (uint16_t) ((character_code + idDelta) % 65536);
            } else {
                const uint32_t glyph_offset =
                    idRangeOffset / 2 + (character_code - startCode) + i;
                glyph_index = getUInt16(buffer, glyph_offset);
            }

            // Check if the glyph index is valid; 0 means no glyph
            if (glyph_index != 0) {
                charMap[mapIndex].unicode = character_code;
                charMap[mapIndex].index = glyph_index;
                mapIndex++;
            }
        }
    }
}
