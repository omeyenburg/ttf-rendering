#include "glyf.h"

void parse_simple_glyph(Glyph* glyph,
                        unsigned char* buffer,
                        uint32_t offset,
                        int16_t numberOfContours) {
    // Allocate end points
    glyph->endPtsOfContours = (uint16_t*) malloc(numberOfContours * sizeof(uint16_t));

    // Check if memory allocation was successful
    if (glyph->endPtsOfContours == NULL) {
        fprintf(stderr, "Failed to allocate memory for glyph points.\n");
        exit(1);
    }

    // Populate end points and get total number of points from last index
    // uint32_t num_points;
    for (int j = 0; j < numberOfContours; j++) {
        uint16_t index = getInt16(buffer, offset + 2 * j);
        glyph->endPtsOfContours[j] = index;
        // num_points = index;
    }
    uint32_t num_points = glyph->endPtsOfContours[numberOfContours - 1] + 1;
    glyph->numPoints = num_points;

    // Skip instructions
    uint16_t instructionLength = getInt16(buffer, offset + 2 * numberOfContours);
    offset += 2 + 2 * numberOfContours + instructionLength;

    uint8_t flags[num_points];
    uint8_t repeat = 0;
    uint32_t y_offset = 0;

    for (int j = 0; j < num_points; j++) {
        if (repeat > 0) {
            flags[j] = flags[j - 1];
            if (flags[j] & 0x02) {
                y_offset += 1;
            } else {
                y_offset += 2;
            }
            repeat -= 1;
            continue;
        }

        flags[j] = buffer[offset];
        if (flags[j] &
            0x02) { // Shift start of y coordinates back depending on x coordinate size
            y_offset += 1;
        } else if (!(flags[j] & 0x10)) {
            y_offset += 2;
        }
        if (flags[j] & 0x08) {
            offset += 1;
            repeat = buffer[offset];
        }
        offset += 1;
    }

    int32_t x_offset = offset;
    y_offset += offset;

    // Allocate points
    glyph->points = (Point*) malloc(num_points * sizeof(Point));
    if (glyph->points == NULL) {
        // Handle memory allocation failure (optional)
        fprintf(stderr, "Failed to allocate memory for glyph points.\n");
        exit(1);
    }

    int16_t last_x = 0;
    int16_t last_y = 0;
    for (int j = 0; j < num_points; j++) {
        int16_t x;
        int16_t y;
        uint8_t onCurve = flags[j] & 0x01 ? 1 : 0;

        // X coordinate
        if (flags[j] & 0x02) {
            // X short vector
            x = buffer[x_offset];
            if (!(flags[j] & 0x10)) {
                x *= -1;
            }
            x_offset += 1;
        } else {
            if (flags[j] & 0x10) {
                x = 0;
            } else {
                x = getInt16(buffer, x_offset);
                x_offset += 2;
            }
        }

        // Y coordinate
        if (flags[j] & 0x04) {
            // Y short vector
            y = buffer[y_offset];
            if (!(flags[j] & 0x20)) {
                y *= -1;
            }
            y_offset += 1;
        } else {
            if (flags[j] & 0x20) {
                y = 0;
            } else {
                y = getInt16(buffer, y_offset);
                y_offset += 2;
            }
        }

        x += last_x;
        y += last_y;
        last_x = x;
        last_y = y;

        glyph->points[j].x = x;
        glyph->points[j].y = y;
        glyph->points[j].onCurve = onCurve;
    }
}

void parse_compound_glyph(Glyph* glyph,
                          unsigned char* buffer,
                          Table* glyf,
                          uint32_t offset,
                          uint32_t* glyf_offsets,
                          uint16_t numGlyphs) {
    uint32_t counting_offset = offset;
    uint16_t numCompounds = 0;
    uint16_t flags;
    do {
        numCompounds++;
        flags = getInt16(buffer, counting_offset);
        counting_offset += 4;

        // Handle the argument types based on the flags
        if (flags & 0x0001) {
            // ARG_1_AND_2_ARE_WORDS: arguments are 16-bit
            counting_offset += 4; // Two 16-bit arguments
        } else {
            // Arguments are 8-bit
            counting_offset += 2; // Two 8-bit arguments
        }

        // Handle optional transformations
        if (flags & 0x0008) {
            // WE_HAVE_A_SCALE: single scale value (2 bytes)
            counting_offset += 2;
        } else if (flags & 0x0040) {
            // WE_HAVE_AN_X_AND_Y_SCALE: two 16-bit scale values (4 bytes)
            counting_offset += 4;
        } else if (flags & 0x0080) {
            // WE_HAVE_A_TWO_BY_TWO: 2x2 transformation matrix (8 bytes)
            counting_offset += 8;
        }

        // Continue looping if the MORE_COMPONENTS flag (0x0020) is set
    } while (flags & 0x0020);

    int16_t total_contours = 0;
    uint16_t total_points = 0;
    uint16_t compound_length[numCompounds];
    uint16_t compound_end_length[numCompounds];
    Point* compound_points[numCompounds];
    uint16_t* compound_end_points[numCompounds];

    for (int i = 0; i < numCompounds; i++) {
        flags = getInt16(buffer, offset);
        uint16_t glyphIndex = getInt16(buffer, offset + 2);
        offset += 4; // Move past the flags and glyphIndex

        Glyph child =
            parse_single_glyph(buffer, glyf, glyf_offsets, numGlyphs, glyphIndex);

        int16_t arg1, arg2;

        // Check if arguments are words or bytes
        if (flags & 0x0001) {
            // ARG_1_AND_2_ARE_WORDS - arguments are 16-bit
            arg1 = getInt16(buffer, offset);
            arg2 = getInt16(buffer, offset + 2);
            offset += 4;
        } else {
            // Arguments are 8-bit
            arg1 = (int16_t) buffer[offset];
            arg2 = (int16_t) buffer[offset + 1];
            offset += 2;
        }

        // If ARGS_ARE_XY_VALUES flag is set, treat as x and y displacement
        if (flags & 0x0002) {
            // arg1, arg2 are x and y offsets
            for (int j = 0; j < child.numPoints; j++) {
                child.points[j].x += arg1;
                child.points[j].y += arg2;
            }
        } else {
            // arg1, arg2 are point numbers
            int j;
            for (j = 0; j < i; j++) {
                if (arg1 > compound_length[j]) {
                    arg1 -= compound_length[j];
                } else {
                    break;
                }
            }
            int16_t x1 = compound_points[j][arg1].x;
            int16_t y1 = compound_points[j][arg1].y;
            int16_t x2 = child.points[arg2].x;
            int16_t y2 = child.points[arg2].y;

            arg1 = x2 - x1;
            arg2 = y2 - y1;

            for (int j = 0; j < child.numPoints; j++) {
                child.points[j].x += arg1;
                child.points[j].y += arg2;
            }

            // TODO: Testing needed! do some prints if any font even uses this feature
        }

        // Check for transformation flags (scaling/rotation)
        if (flags & 0x0008) {
            // WE_HAVE_A_SCALE - Single scale value
            // TODO: test if this should be 65536.0 (16.16 or 2.14), but for 16.16
            // format it would need a 32 int
            float scale = getInt16(buffer, offset) / 16384.0;
            offset += 2;

            for (int j = 0; j < child.numPoints; j++) {
                child.points[j].x *= scale;
                child.points[j].y *= scale;
            }
        } else if (flags & 0x0040) {
            // WE_HAVE_AN_X_AND_Y_SCALE - Separate x, y scaling
            float xscale = getInt16(buffer, offset) / 16384.0;
            float yscale = getInt16(buffer, offset + 2) / 16384.0;
            offset += 4;

            for (int j = 0; j < child.numPoints; j++) {
                child.points[j].x *= xscale;
                child.points[j].y *= yscale;
            }
        } else if (flags & 0x0080) {
            // WE_HAVE_A_TWO_BY_TWO - 2x2 transformation matrix
            float xscale = getInt16(buffer, offset) / 16384.0;
            float scale01 = getInt16(buffer, offset + 2) / 16384.0;
            float scale10 = getInt16(buffer, offset + 4) / 16384.0;
            float yscale = getInt16(buffer, offset + 6) / 16384.0;
            offset += 8;

            for (int j = 0; j < child.numPoints; j++) {
                int16_t x = child.points[j].x;
                int16_t y = child.points[j].y;
                child.points[j].x = xscale * x + scale10 * y;
                child.points[j].y = yscale * y + scale01 * x;
            }
        }

        total_points += child.numPoints;
        total_contours += child.numberOfContours;

        compound_length[i] = child.numPoints;
        compound_points[i] = (Point*) malloc(child.numPoints * sizeof(Point));
        if (compound_points[i] == NULL) {
            fprintf(stderr, "Failed to allocate memory for glyph points.\n");
            exit(1);
        }
        for (int j = 0; j < child.numPoints; j++) {
            compound_points[i][j] = child.points[j];
        }

        compound_end_length[i] = child.numberOfContours;
        compound_end_points[i] =
            (uint16_t*) malloc(child.numberOfContours * sizeof(uint16_t));
        if (compound_end_points[i] == NULL) {
            fprintf(stderr, "Failed to allocate memory for glyph end points.\n");
            exit(1);
        }
        for (int j = 0; j < child.numberOfContours; j++) {
            compound_end_points[i][j] = child.endPtsOfContours[j];
        }
    }

    glyph->numPoints = total_points;
    glyph->numberOfContours = 0;
    glyph->points = (Point*) malloc(total_points * sizeof(Point));
    if (glyph->points == NULL) {
        fprintf(stderr, "Failed to allocate memory for glyph points.\n");
        exit(1);
    }
    glyph->endPtsOfContours = (uint16_t*) malloc(total_contours * sizeof(uint16_t));
    if (glyph->endPtsOfContours == NULL) {
        fprintf(stderr, "Failed to allocate memory for glyph end points.\n");
        exit(1);
    }

    uint16_t point_index = 0;
    for (int i = 0; i < numCompounds; i++) {
        for (int j = 0; j < compound_length[i]; j++) {
            glyph->points[point_index++] = compound_points[i][j];
        }
    }
}

Glyph parse_single_glyph(unsigned char* buffer,
                         Table* glyf,
                         uint32_t* glyf_offsets,
                         uint16_t numGlyphs,
                         uint16_t index) {
    uint32_t offset = glyf->offset + glyf_offsets[index];
    if (offset >= glyf->offset + glyf->length) {
        fprintf(stderr, "Glyph offset out of range for glyph index %d\n", index);
        exit(1);
    }

    int16_t numberOfContours = getInt16(buffer, offset);

    Glyph glyph;
    glyph.numberOfContours = numberOfContours;
    glyph.xMin = getInt16(buffer, offset + 2);
    glyph.yMin = getInt16(buffer, offset + 4);
    glyph.xMax = getInt16(buffer, offset + 6);
    glyph.yMax = getInt16(buffer, offset + 8);
    // printf("%i\n", numberOfContours);
    // if (index == 36) {
    //     printf("Box: %i, %i, %i, %i\n", glyph.xMin, glyph.yMin, glyph.xMax,
    //     glyph.yMax);
    // }
    offset += 10;

    if (numberOfContours == 0) {
        // printf("No contours for glyph %i.\n", index); // it's ok if some glyphs have
        // no contours, don't print to avoid spamming
    } else if (numberOfContours > 0) {
        // Simple glyph
        parse_simple_glyph(&glyph, buffer, offset, numberOfContours);
    } else if (numberOfContours == -1) {
        // Compound glyph
        parse_compound_glyph(&glyph, buffer, glyf, offset, glyf_offsets, numGlyphs);
    } else {
        // fprintf(
        //     stderr, "Encountered unexpected numberOfContours: %i.\n",
        //     numberOfContours);
        // exit(1);
        // fprintf(stderr, "Ignoring this error.\n");
        //
        // We don't print here, it is too much output... TODO: check if this is fixed
        // later
    }

    return glyph;
}

void parse_glyf(Glyph* glyphs,
                unsigned char* buffer,
                Table* glyf,
                uint32_t* glyf_offsets,
                uint16_t numGlyphs) {
    if (!glyf->initialized) {
        fprintf(stderr, "Table \"glyf\" was not found!\n");
        exit(1);
    }

    for (int i = 0; i < numGlyphs; i++) {
        glyphs[i] = parse_single_glyph(buffer, glyf, glyf_offsets, numGlyphs, i);
        // printf("Index %i\thas %i points\n", i, glyphs[i].numPoints);
    }
}
