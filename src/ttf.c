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

void load(char* path) {
    printf("Loading font %s\n", path);

    FILE* fp;
    size_t size;
    unsigned char* buffer;

    fp = fopen(path, "rb");
    if (fp == NULL) { // Check if file opened successfully
        printf("Error: There was an error opening the file %s \n", path);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    if (size == 0) { // Check if file size is zero
        printf("Error: The file %s is empty.\n", path);
        fclose(fp);
        exit(1);
    }

    buffer = malloc(size);
    if (buffer == NULL) { // Check if malloc was successful
        printf("Error: Memory allocation failed.\n");
        fclose(fp);
        exit(1);
    }

    if (fread(buffer, 1, size, fp) != size) { // Correct read statement
        printf(
            "Error: There was an error reading the file %s \nUnexpected file size.\n",
            path);
        free(buffer);
        fclose(fp);
        exit(1);
    } else {
        for (size_t i = 0; i < size; i++) {
            // char c = buffer[i];
            //  printf("%02x ", buffer[i]);
            //  printf("%c ", c);
        }
        printf("\n"); // Print a newline for better formatting
    }

    uint32_t version = getInt32(buffer, 0);
    printf("Version: %i\n", version);

    uint16_t numTables = getInt16(buffer, 4);
    printf("Num tables: %i\n", numTables);

    Table cmap;
    Table glyf;
    Table head;
    Table htmx;
    Table loca;
    Table maxp;

    int buffer_offset = 12;
    for (int i = 0; i < numTables; i++) {
        const char* label = bytes4xchar(buffer, buffer_offset);
        uint32_t checkSum = getInt32(buffer, buffer_offset + 4);
        uint32_t offset = getInt32(buffer, buffer_offset + 8);
        uint32_t length = getInt32(buffer, buffer_offset + 12);
        // printf("Label: %s\t", label);
        // printf("Check sum: %u\t", checkSum);
        // printf("Offset: %u\t", offset);
        // printf("Length: %u\n", length);

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
        } else if (strcmp(label, "htmx") == 0) {
            htmx.initialized = true;
            htmx.checkSum = checkSum;
            htmx.offset = offset;
            htmx.length = length;
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
    uint16_t numGlyphs = parse_maxp(buffer, &maxp); // maybe add 1

    printf("locFormat: %i, numGlyphs: %i\n", indexToLocFormat, numGlyphs);

    // Relative offsets in glyf; first index used for missing character glyph
    uint32_t glyf_offsets[numGlyphs];
    Glyph glyphs[numGlyphs];
    uint32_t characters[255];

    parse_loca(glyf_offsets, buffer, &loca, numGlyphs, indexToLocFormat);
    parse_glyf(glyphs, buffer, &glyf, glyf_offsets, numGlyphs);
    parse_cmap(characters, buffer, &cmap, glyf_offsets);
    parse_htmx(buffer, &htmx);

    fclose(fp);
    free(buffer);

    int n = 0;
    for (int i = 0; i < 255; i++) {
        if (i == '¨') {
            n = characters[i];
            break;
        }
    }

    // printf("A in ascii is: %i\n", 'A');
    // int n = 36;
    for (int i = 0; i < glyphs[n].numPoints; i++) {
        printf("(%i, %i), ", glyphs[n].points[i].x, glyphs[n].points[i].y);
    }
    printf("\n");
    printf("A has %i points\n", glyphs[n].numPoints);
    printf("A has %i contours\n", glyphs[n].numberOfContours);
}
