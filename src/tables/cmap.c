#include "../ttf.h"

uint16_t get_cmap_size(unsigned char* buffer, Table* cmap) {
    if (!cmap->initialized) {
        fprintf(stderr, "Table \"cmap\" was not found!\n");
        exit(1);
    }

    u_int16_t version = getInt16(buffer, cmap->offset);
    if (version != 0) {
        fprintf(stderr, "Expected CMAP version 0, got %i.", version);
        exit(1);
    }

    u_int16_t numSubtables = getInt16(buffer, cmap->offset + 2);
    if (numSubtables == 0) {
        fprintf(stderr, "Could not find any sub tables in CMAP table.");
        exit(1);
    }

    uint16_t platform_id = 4;
    uint16_t platform_specific_id = 0;
    uint32_t offset = 0;
    uint32_t buffer_offset = cmap->offset + 4;
    for (int i = 0; i < numSubtables; i++) {
        uint16_t sub_platform_id = getInt16(buffer, buffer_offset);
        uint16_t sub_platform_specific_id = getInt16(buffer, buffer_offset + 2);
        uint32_t sub_offset = getInt32(buffer, buffer_offset + 4);

        if (platform_id == 4 || platform_id != 0 ||
            (platform_id == 1 && sub_platform_id == 3)) {
            platform_id = sub_platform_id;
            platform_specific_id = sub_platform_specific_id;
            offset = cmap->offset + sub_offset;
        }

        buffer_offset += 8;
    }

    uint16_t format = getInt16(buffer, offset);
    if (format != 4) {
        fprintf(stderr,
                "Unsupported format %i. Currently only format 4 is supported.",
                format);
        exit(1);
    }

    uint16_t length = getInt16(buffer, offset + 2);
    uint16_t segCount = getInt16(buffer, offset + 6) / 2;
    uint32_t charCount = 0;
    for (int i = 0; i < segCount; i++) {
        uint16_t endCode = getInt16(buffer, offset + 14 + i * 2);
        uint16_t startCode = getInt16(buffer, offset + 16 + segCount * 2 + i * 2);
        if (startCode <= endCode) {
            // Number of characters in this segment
            charCount += (endCode - startCode + 1);
        }
    }

    return charCount;
}

void parse_cmap(CharacterMap* charMap,
                unsigned char* buffer,
                Table* cmap,
                uint16_t numChars) {
    if (!cmap->initialized) {
        fprintf(stderr, "Table \"cmap\" was not found!\n");
        exit(1);
    }

    u_int16_t version = getInt16(buffer, cmap->offset);
    if (version != 0) {
        fprintf(stderr, "Expected CMAP version 0, got %i.", version);
        exit(1);
    }

    u_int16_t numSubtables = getInt16(buffer, cmap->offset + 2);
    if (numSubtables == 0) {
        fprintf(stderr, "Could not find any sub tables in CMAP table.");
        exit(1);
    }

    uint16_t platform_id = 4;
    uint16_t platform_specific_id = 0;
    uint32_t offset = 0;
    uint32_t buffer_offset = cmap->offset + 4;
    for (int i = 0; i < numSubtables; i++) {
        uint16_t sub_platform_id = getInt16(buffer, buffer_offset);
        uint16_t sub_platform_specific_id = getInt16(buffer, buffer_offset + 2);
        uint32_t sub_offset = getInt32(buffer, buffer_offset + 4);

        if (platform_id == 4 || platform_id != 0 ||
            (platform_id == 1 && sub_platform_id == 3)) {
            platform_id = sub_platform_id;
            platform_specific_id = sub_platform_specific_id;
            offset = cmap->offset + sub_offset;
        }

        buffer_offset += 8;
    }
    printf("Platform ID: %i, ", platform_id);
    printf("Platform specific ID: %i, ", platform_specific_id);
    printf("Offset: %i\n", offset);

    // if (platform_id != 0 && platform_id != 3) {
    //     fprintf(stderr, "Unsupported platform %i\n", platform_id);
    //     exit(1);
    // }

    uint16_t format = getInt16(buffer, offset);
    printf("Format: %i\n", format);

    if (format != 4) {
        fprintf(stderr,
                "Unsupported format %i. Currently only format 4 is supported.",
                format);
        exit(1);
    }

    // length of entire subtable in bytes
    uint16_t length = getInt16(buffer, offset + 2);

    uint16_t language = getInt16(buffer, offset + 4);
    uint16_t segCountX2 = getInt16(buffer, offset + 6);
    uint16_t segCount = segCountX2 / 2;

    // Read reservedPad (2 bytes) - it is always 0
    uint16_t reservedPad = getInt16(buffer, offset + 14 + segCount * 2);
    if (reservedPad != 0) {
        fprintf(stderr, "Control value reservedPad should always be 0!");
        exit(1);
    }

    // Read the idDelta array (shift of a character code to get the glyph index)
    int16_t* idDelta = malloc(segCount * sizeof(int16_t));
    for (int i = 0; i < segCount; i++) {
        idDelta[i] = getInt16(buffer, offset + 16 + segCount * 4 + i * 2);
    }

    uint16_t* idRangeOffset = malloc(segCount * sizeof(uint16_t));
    for (int i = 0; i < segCount; i++) {
        idRangeOffset[i] = getInt16(buffer, offset + 16 + segCount * 6 + i * 2);
    }

    // Read the glyphIndexArray
    uint16_t* glyphIndexArray =
        malloc((length - 16 - 4 * segCountX2) * sizeof(uint16_t));
    uint32_t glyph_index_offset = offset + 16 + segCount * 6;
    for (int i = 0; i < length - 16 - 4 * segCountX2; i += 2) {
        glyphIndexArray[i / 2] = getInt16(buffer, glyph_index_offset + i);
    }

    // Read the segment start and end points and count characters
    int num_glyphs = 0;
    uint32_t charCount = 0;
    uint32_t lastCharacter = 0;
    uint16_t* endCode = malloc(segCount * sizeof(uint16_t));
    uint16_t* startCode = malloc(segCount * sizeof(uint16_t));
    uint16_t mapIndex = 0;
    for (int i = 0; i < segCount; i++) {
        endCode[i] = getInt16(buffer, offset + 14 + i * 2);
        startCode[i] = getInt16(buffer, offset + 16 + segCount * 2 + i * 2);
        if (startCode[i] <= endCode[i]) {
            // Number of characters in this segment
            charCount += (endCode[i] - startCode[i] + 1);
        }
        if (endCode[i] > lastCharacter) {
            lastCharacter = endCode[i];
        }
        for (uint32_t character_code = startCode[i]; character_code <= endCode[i];
             character_code++) {
            uint16_t glyph_index;
            if (idRangeOffset[i] == 0) {
                glyph_index = (uint16_t) ((character_code + idDelta[i]) % 65536);
            } else {
                uint32_t glyph_offset =
                    idRangeOffset[i] / 2 + (character_code - startCode[i]) + i;
                glyph_index = getInt16(buffer, glyph_offset);
                // glyph_index =
                //     *(idRangeOffset[i] / 2 + (character_code - startCode[i])
                //     +
                //       &idRangeOffset[i]);
            }

            // Check if the glyph index is valid
            if (glyph_index != 0) { // Glyph index 0 means no glyph
                // characters[character_code] = glyph_index;
                // printf("Character Code: %u => Glyph Index: %u\n",
                //        character_code,
                //        glyph_index);
                charMap[mapIndex].unicode = character_code;
                charMap[mapIndex].index = glyph_index;
                mapIndex++;
                num_glyphs += 1;
            }
        }
    }
    printf("Found chars: %i --> %i\n", charCount, lastCharacter);

    // Print out the mapping for character codes to glyph indices
    // for (uint16_t character_code = 0; character_code <= 255; character_code++) {
    //     characters[character_code] = 0;
    //     for (int i = 0; i < segCount; i++) {
    //         if (character_code >= startCode[i] && character_code <= endCode[i]) {
    //             uint16_t glyph_index;
    //             if (idRangeOffset[i] == 0) {
    //                 glyph_index = (uint16_t) ((character_code + idDelta[i]) % 65536);
    //             } else {
    //                 uint32_t glyph_offset =
    //                     idRangeOffset[i] / 2 + (character_code - startCode[i]) + i;
    //                 glyph_index = getInt16(buffer, glyph_offset);
    //                 // glyph_index =
    //                 //     *(idRangeOffset[i] / 2 + (character_code - startCode[i])
    //                 //     +
    //                 //       &idRangeOffset[i]);
    //             }
    //
    //             // Check if the glyph index is valid
    //             if (glyph_index != 0) { // Glyph index 0 means no glyph
    //                 characters[character_code] = glyph_index;
    //                 // printf("Character Code: %u => Glyph Index: %u\n",
    //                 //        character_code,
    //                 //        glyph_index);
    //                 num_glyphs += 1;
    //             }
    //             break;
    //         }
    //     }
    // }
    printf("Indexed %i glyphs successfully\n", num_glyphs);

    // Free allocated memory
    free(endCode);
    free(startCode);
    free(idDelta);
    free(glyphIndexArray);
}
