#include "../ttf.h"

void parse_cmap(unsigned char* buffer, Table* cmap) {
    if (!cmap->initialized) {
        fprintf(stderr, "Table \"cmap\" was not found!\n");
        exit(1);
    }
    u_int16_t version = getInt16(buffer, cmap->offset);
    printf("CMAP\nVersion: %i\n", version);
    u_int16_t numSubtables = getInt16(buffer, cmap->offset + 2);
    printf("Number Subtables: %i\n", numSubtables);

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
    printf(
        "Length: %i, Language: %i, Seg Count x2: %i\n", length, language, segCountX2);

    // Read the segment end points
    uint16_t* endCount = malloc(segCount * sizeof(uint16_t));
    for (int i = 0; i < segCount; i++) {
        endCount[i] = getInt16(buffer, offset + 14 + (i * 2));
    }

    // Read reservedPad (2 bytes) - it is always 0
    uint16_t reservedPad = getInt16(buffer, offset + 14 + segCount * 2);
    if (reservedPad != 0) {
        fprintf(stderr, "Control value reservedPad should always be 0!");
        exit(1);
    }

    // Read the startCount array (segment start points)
    uint16_t* startCode = malloc(segCount * sizeof(uint16_t));
    for (int i = 0; i < segCount; i++) {
        startCode[i] = getInt16(buffer, offset + 16 + segCount * 2 + (i * 2));
    }

    // Read the idDelta array (how much to add to a character code to get the glyph
    // index)
    int16_t* idDelta = malloc(segCount * sizeof(int16_t));
    for (int i = 0; i < segCount; i++) {
        idDelta[i] = getInt16(buffer, offset + 16 + segCount * 4 + (i * 2));
    }

    uint16_t* idRangeOffset = malloc(segCount * sizeof(uint16_t));
    for (int i = 0; i < segCount; i++) {
        idRangeOffset[i] = getInt16(buffer, offset + 16 + segCount * 6 + (i * 2));
    }

    // Read the glyphIndexArray
    uint16_t* glyphIndexArray =
        malloc((length - 16 - 4 * segCountX2) * sizeof(uint16_t));
    uint32_t glyph_index_offset = offset + 16 + segCount * 6;
    for (int i = 0; i < length - 16 - 4 * segCountX2; i += 2) {
        glyphIndexArray[i / 2] = getInt16(buffer, glyph_index_offset + i);
    }

    // Print out the mapping for character codes to glyph indices
    int num_glyphs = 0;
    for (uint16_t character_code = 0; character_code <= 255; character_code++) {
        for (int i = 0; i < segCount; i++) {
            if (character_code >= startCode[i] && character_code <= endCount[i]) {
                uint16_t glyph_index;
                if (idRangeOffset[i] == 0) {
                    glyph_index = (uint16_t) ((character_code + idDelta[i]) % 65536);
                } else {
                    uint32_t glyph_offset =
                        idRangeOffset[i] / 2 + (character_code - startCode[i]) + i;
                    glyph_index = getInt16(buffer, glyph_offset);
                    // glyph_index =
                    //     *(idRangeOffset[i] / 2 + (character_code - startCode[i]) +
                    //       &idRangeOffset[i]);
                }

                // Check if the glyph index is valid
                if (glyph_index != 0) { // Glyph index 0 means no glyph
                    // printf("Character Code: %u => Glyph Index: %u\n", character_code,
                    // glyph_index);
                    num_glyphs += 1;
                }
                break;
            }
        }
    }
    printf("Indexed %i glyphs successfully\n", num_glyphs);

    // Free allocated memory
    free(endCount);
    free(startCode);
    free(idDelta);
    free(glyphIndexArray);
}
