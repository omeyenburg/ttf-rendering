#include "ttf.h"

char* bytes4xchar(unsigned char* buffer, int offset) {
    static char string[5];
    string[0] = buffer[offset];
    string[1] = buffer[offset + 1];
    string[2] = buffer[offset + 2];
    string[3] = buffer[offset + 3];
    string[4] = '\0';
    return string;
}

uint16_t unicode(unsigned char* c) {
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

bool validateCheckSum(unsigned char* buffer, Table* table, uint32_t adjustment) {
    uint32_t full_parts = table->length / 4;
    uint32_t remainder = table->length - full_parts * 4;

    uint32_t sum = 0;
    for (int i = 0; i < full_parts; i++) {
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

void load(char* path) {
    printf("Loading font %s\n", path);

    FILE* fp;
    size_t size;
    unsigned char* buffer;

    fp = fopen(path, "rb");

    // Check if file opened successfully
    if (fp == NULL) {
        printf("Error: There was an error opening the file %s \n", path);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Check if file size is zero
    if (size == 0) {
        printf("Error: The file %s is empty.\n", path);
        fclose(fp);
        exit(1);
    }

    buffer = malloc(size);

    // Check if malloc was successful
    if (buffer == NULL) {
        printf("Error: Memory allocation failed.\n");
        fclose(fp);
        exit(1);
    }

    if (fread(buffer, 1, size, fp) != size) {
        printf(
            "Error: There was an error reading the file %s \nUnexpected file size.\n",
            path);
        free(buffer);
        fclose(fp);
        exit(1);
    }

    // Check version
    uint32_t version = getUInt32(buffer, 0);
    if (version != 65536) {
        fprintf(stderr, "Unexpected font version: %i\n", version);
        exit(1);
    }

    // Get number of tables
    uint16_t numTables = getUInt16(buffer, 4);

    // Read table offsets
    Table cmap;
    Table glyf;
    Table head;
    Table hhea;
    Table hmtx;
    Table loca;
    Table maxp;
    int buffer_offset = 12;
    for (int i = 0; i < numTables; i++) {
        const char* label = bytes4xchar(buffer, buffer_offset);
        uint32_t checkSum = getUInt32(buffer, buffer_offset + 4);
        uint32_t offset = getUInt32(buffer, buffer_offset + 8);
        uint32_t length = getUInt32(buffer, buffer_offset + 12);

        if (strcmp(label, "cmap") == 0) {
            cmap.initialized = true;
            cmap.checkSum = checkSum;
            cmap.offset = offset;
            cmap.length = length;
        } else if (strcmp(label, "glyf") == 0) {
            glyf.initialized = true;
            glyf.checkSum = checkSum;
            glyf.offset = offset;
            glyf.length = length;
        } else if (strcmp(label, "head") == 0) {
            head.initialized = true;
            head.checkSum = checkSum;
            head.offset = offset;
            head.length = length;
        } else if (strcmp(label, "hhea") == 0) {
            hhea.initialized = true;
            hhea.checkSum = checkSum;
            hhea.offset = offset;
            hhea.length = length;
        } else if (strcmp(label, "hmtx") == 0) {
            hmtx.initialized = true;
            hmtx.checkSum = checkSum;
            hmtx.offset = offset;
            hmtx.length = length;
        } else if (strcmp(label, "loca") == 0) {
            loca.initialized = true;
            loca.checkSum = checkSum;
            loca.offset = offset;
            loca.length = length;
        } else if (strcmp(label, "maxp") == 0) {
            maxp.initialized = true;
            maxp.checkSum = checkSum;
            maxp.offset = offset;
            maxp.length = length;
        }

        buffer_offset += 16;
    }

    int16_t indexToLocFormat = parse_head(buffer, &head);
    uint16_t numGlyphs = parse_maxp(buffer, &maxp);

    // Relative offsets in glyf
    uint32_t glyf_offsets[numGlyphs];
    Glyph glyphs[numGlyphs];

    // Get number of mapped characters
    uint16_t numChars = get_cmap_size(buffer, &cmap);
    CharacterMap charMap[numChars];

    // Get glyph data
    parse_loca(glyf_offsets, buffer, &loca, numGlyphs, indexToLocFormat);
    parse_glyf(glyphs, buffer, &glyf, glyf_offsets, numGlyphs);
    parse_cmap(charMap, buffer, &cmap, numChars);

    // Get horizontal metrics data
    uint16_t numberOfHMetrics = parse_hhea(buffer, &hhea);
    uint16_t advanceWidth[numberOfHMetrics];
    int16_t leftSideBearings[numGlyphs];
    parse_hmtx(
        advanceWidth, leftSideBearings, buffer, &hmtx, numberOfHMetrics, numGlyphs);
    // DOCS: As an optimization, the number of records can be less than the number of
    // glyphs, in which case the advance width value of the last record applies to all
    // remaining glyph IDs. This can be useful in monospaced fonts, or in fonts that
    // have a large number of glyphs with the same advance width (provided the glyphs
    // are ordered appropriately).

    // Close font file and deallocate buffer
    fclose(fp);
    free(buffer);

    // Get glyph index n from character
    int n = 0;
    unsigned char c[] = "Ä";
    uint16_t u = unicode(c);
    for (int i = 0; i < numChars; i++) {
        if (charMap[i].unicode == u) {
            n = charMap[i].index;
            break;
        }
    }

    // TODO: Add points directly in the center between consecutive offCurve points
    //       to turn cubic beziers into quadratic beziers.
    //       Maybe even add offCurve points between onCurve points to reduce shader
    //       branching. Might be irrelevant due to looping over lines/beziers.

    // Print out points of glyph n
    for (int i = 0; i < glyphs[n].numPoints; i++) {
        printf("(%i, %i), ", glyphs[n].points[i].x, glyphs[n].points[i].y);
    }
    printf("\n");

    // Deallocate memory
    for (int i = 0; i < numGlyphs; i++) {
        free(glyphs[i].points);
        free(glyphs[i].endPtsOfContours);
    }
}
