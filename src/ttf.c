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
    uint32_t version = getInt32(buffer, 0);
    if (version != 65536) {
        fprintf(stderr, "Unexpected font version: %i\n", version);
        exit(1);
    }

    // Get number of tables
    uint16_t numTables = getInt16(buffer, 4);

    // Read table offsets
    Table cmap;
    Table glyf;
    Table head;
    Table hmtx;
    Table loca;
    Table maxp;
    int buffer_offset = 12;
    for (int i = 0; i < numTables; i++) {
        const char* label = bytes4xchar(buffer, buffer_offset);
        uint32_t checkSum = getInt32(buffer, buffer_offset + 4);
        uint32_t offset = getInt32(buffer, buffer_offset + 8);
        uint32_t length = getInt32(buffer, buffer_offset + 12);

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

    // Get font data
    parse_loca(glyf_offsets, buffer, &loca, numGlyphs, indexToLocFormat);
    parse_glyf(glyphs, buffer, &glyf, glyf_offsets, numGlyphs);
    parse_cmap(charMap, buffer, &cmap, numChars);
    parse_hmtx(buffer, &hmtx);

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
