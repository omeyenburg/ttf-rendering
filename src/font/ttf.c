#include "font/ttf.h"
#include "font/font.h"
#include "font/tables/cmap.h"
#include "font/tables/glyf.h"
#include "font/tables/head.h"
#include "font/tables/hhea.h"
#include "font/tables/hmtx.h"
#include "font/tables/loca.h"
#include "font/tables/maxp.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void load_ttf(const char* path) {
    printf("Loading font %s\n", path);

    FILE* fp = fopen(path, "rb");

    // Check if file opened successfully
    if (fp == NULL) {
        printf("Error: There was an error opening the file %s \n", path);
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    const size_t size = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    // Check if file size is zero
    if (size == 0) {
        printf("Error: The file %s is empty.\n", path);
        fclose(fp);
        exit(1);
    }

    unsigned char* buffer = (unsigned char*) malloc(size);

    // Check if malloc was successful
    if (buffer == NULL) {
        printf("Error: Memory allocation failed.\n");
        fclose(fp);
        exit(1);
    }

    // Read file content into buffer
    if (fread(buffer, 1, size, fp) != size) {
        printf(
            "Error: There was an error reading the file %s \nUnexpected file size.\n",
            path);
        free(buffer);
        buffer = NULL;
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
    size_t buffer_offset = 12;
    for (size_t i = 0; i < numTables; i++) {
        const char* label = bytes4xchar(buffer, buffer_offset);
        const uint32_t checkSum = getUInt32(buffer, buffer_offset + 4);
        const uint32_t offset = getUInt32(buffer, buffer_offset + 8);
        const uint32_t length = getUInt32(buffer, buffer_offset + 12);

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

    const int16_t indexToLocFormat = parse_head(buffer, &head);
    const uint16_t numGlyphs = parse_maxp(buffer, &maxp);

    // Relative offsets in glyf
    uint32_t* glyf_offsets = (uint32_t*) malloc(sizeof(uint32_t) * numGlyphs);
    Glyph* glyphs = (Glyph*) malloc(sizeof(Glyph) * numGlyphs);

    // Check if malloc was successful
    if (glyf_offsets == NULL) {
        printf("Error: Memory allocation failed.\n");
        exit(1);
    }

    if (glyphs == NULL) {
        printf("Error: Memory allocation failed.\n");
        exit(1);
    }

    // Get number of mapped characters
    const uint16_t numChars = get_cmap_size(buffer, &cmap);
    CharacterMap charMap[numChars];

    // Get glyph data
    parse_loca(glyf_offsets, buffer, &loca, numGlyphs, indexToLocFormat);
    parse_glyf(glyphs, buffer, &glyf, glyf_offsets, numGlyphs);

    // Deallocate memory for offests in glyf table
    free(glyf_offsets);
    glyf_offsets = NULL;

    parse_cmap(charMap, buffer, &cmap, numChars);

    // Get horizontal metrics data
    const uint16_t numberOfHMetrics = parse_hhea(buffer, &hhea);

    // Allocate arrays for hmtx output  TODO: these are not yet used.
    uint16_t* advanceWidth = (uint16_t*) malloc(sizeof(uint16_t) * numberOfHMetrics);
    int16_t* leftSideBearings = (int16_t*) malloc(sizeof(int16_t) * numGlyphs);

    // Check if malloc was successful
    if (advanceWidth == NULL) {
        printf("Error: Memory allocation failed.\n");
        exit(1);
    }

    if (leftSideBearings == NULL) {
        printf("Error: Memory allocation failed.\n");
        exit(1);
    }

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
    buffer = NULL;

    // Get glyph index n from character
    int n = 0;
    unsigned char c[] = "Ä";
    uint16_t u = unicode(c);
    for (size_t i = 0; i < numChars; i++) {
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
    for (size_t i = 0; i < glyphs[n].numPoints; i++) {
        printf("(%i, %i), ", glyphs[n].points[i].x, glyphs[n].points[i].y);
    }
    printf("\n");

    /*

    TODO: What to do if there are more than 2¹⁶ points?

    Tex0: Red, 16 bit, int, size = 3 + 2 * numGlyphs + numContours
    int:                numGlyphs + 1
    int[numGlyphs + 1]: |index| in Tex1 to Glyph points; positive sign = onCurve
    TODO: fix: glyph's first point + length

    int[numGlyphs + 1]: |index| in Tex2 to Glyph end point indices, 0 = None
    int[numContours]:   end point index of Glyph points

    Tex1: Red+Green, 16 bit, float, size = numPoints
    vec2[numPoints]:    x and y position of each point

    # NOTE: IMPROVED VERSION

    Tex0: Red, 16 bit, unsigned int, size = 3 + 2 * numGlyphs
    int:                numGlyphs + 1
    int[numGlyphs + 1]: index/2 in Tex1 to Glyph points

    Tex1: Red+Green, 16 bit, Float, size = variable; >= numPoints
    vec2[variable]:    x and y position of each point
    -> negative sign of x: off curve point
    -> negative sign of y: end of contour point
    -> all Glyphs have even number of points; additional point added for 16 bit index


    TODO: LATER WITH SECTIONS
    - 2 columns and n rows -> 2*n rectangles

    */

    // Count missing onCurve points between two offCurve Points
    uint32_t newNumPoints = 0;
    uint32_t numPoints = 0;
    for (size_t i = 0; i < numGlyphs; i++) {
        Glyph* glyph = &glyphs[i];
        for (size_t j = 0; j < glyph->numberOfContours; j++) {
            const int k_start = (j == 0) ? 0 : (glyph->endPtsOfContours[j - 1] + 1);
            const int k_end = glyph->endPtsOfContours[j] + 1;
            if (i == 36) {
                printf("%i, %i, \n", k_start, k_end);
            }
            for (size_t k = k_start; k < k_end; k++) {
                int k_last = (k == k_start) ? (k_end - 1) : (k - 1);
                if (!glyph->points[k].onCurve && !glyph->points[k_last].onCurve) {
                    newNumPoints += 1;
                }
                numPoints += 1;
            }
        }
    }

    printf("Added %i points; %i.\n", newNumPoints, numPoints);

    // Allocate texture data arrays
    size_t tex0_size;
    size_t tex1_size;

    // Deallocate memory
    free(advanceWidth);
    free(leftSideBearings);
    advanceWidth = NULL;
    leftSideBearings = NULL;

    for (size_t i = 0; i < numGlyphs; i++) {
        free(glyphs[i].points);
        free(glyphs[i].endPtsOfContours);
        glyphs[i].points = NULL;
        glyphs[i].endPtsOfContours = NULL;
    }
    free(glyphs);
    glyphs = NULL;
}
