#include "../ttf.h"

void parse_glyf(unsigned char* buffer,
                Table* glyf,
                uint32_t* glyf_offsets,
                uint32_t* glyf_lengths,
                uint16_t numGlyphs) {
    if (!glyf->initialized) {
        fprintf(stderr, "Table \"glyf\" was not found!:\n");
        exit(1);
    }

    Glyph glyphs[numGlyphs];

    for (int i = 0; i < numGlyphs; i++) {
        uint32_t offset = glyf_offsets[i];
        int16_t numberOfContours = getInt16(buffer, offset);
        FWord xMin = getInt16(buffer, offset + 2);
        FWord yMin = getInt16(buffer, offset + 4);
        FWord xMax = getInt16(buffer, offset + 6);
        FWord yMax = getInt16(buffer, offset + 8);

        glyphs[i].numberOfContours = numberOfContours;
        glyphs[i].xMin = xMin;
        glyphs[i].yMin = yMin;
        glyphs[i].xMax = xMax;
        glyphs[i].yMax = yMax;

        if (numberOfContours == 0) {
            // printf("No contours for glyph %i.\n", i);
        } else if (numberOfContours > 0) {
            // Allocate end points
            glyphs[i].endPtsOfContours =
                (uint16_t*) malloc(numberOfContours * sizeof(uint16_t));
            // Check if memory allocation was successful
            if (glyphs[i].endPtsOfContours == NULL) {
                fprintf(stderr, "Failed to allocate memory for glyph points.\n");
                exit(1);
            }

            uint32_t num_points;
            for (int j = 0; j < numberOfContours; j++) {
                uint16_t index = getInt16(buffer, offset + 10 + 2 * j);
                glyphs[i].endPtsOfContours[j] = index;
                num_points = index;
            }
            glyphs[i].numPoints = num_points;

            uint16_t instructionLength =
                getInt16(buffer, offset + 10 + 2 * numberOfContours);
            offset += 12 + 2 * numberOfContours + instructionLength;

            uint8_t flags[num_points];
            uint8_t repeat = 0;
            uint32_t y_offset = 0;

            for (int j = 0; j < num_points; j++) {
                if (repeat > 0) {
                    flags[j] = flags[j - 1];
                    repeat -= 1;
                    continue;
                }

                flags[j] = buffer[offset];
                if (flags[j] & 0x02) {
                    y_offset += 1;
                } else {
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
            glyphs[i].points = (Point*) malloc(num_points * sizeof(Point));
            // Check if memory allocation was successful
            if (glyphs[i].points == NULL) {
                // Handle memory allocation failure (optional)
                fprintf(stderr, "Failed to allocate memory for glyph points.\n");
                exit(1);
            }

            int16_t last_x = 0;
            int16_t last_y = 0;
            for (int j = 0; j < num_points; j++) {
                int16_t x;
                int16_t y;
                uint8_t onCurve;

                if (flags[j] & 0x01) {
                    onCurve = 1;
                } else {
                    onCurve = 0;
                }

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
                    if (!(flags[j] & 0x10)) {
                        y *= -1;
                    }
                    y_offset += 1;
                } else {
                    if (flags[j] & 0x10) {
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

                glyphs[i].points[j].x = x;
                glyphs[i].points[j].y = y;
                glyphs[i].points[j].onCurve = onCurve;
            }
        } else {
            // Compound glyph
            offset += 10;
            uint8_t more_components = 1;
            while (more_components) {
                uint16_t flags = getInt16(buffer, offset);
                uint16_t glyphIndex = getInt16(buffer, offset + 2);
                offset += 4; // Move past the flags and glyphIndex

                int16_t arg1, arg2;

                // Check if arguments are words or bytes
                if (flags & 0x0001) {
                    // ARG_1_AND_2_ARE_WORDS - arguments are 16-bit
                    arg1 = getInt16(buffer, offset);
                    arg2 = getInt16(buffer, offset + 2);
                    offset += 4;
                } else {
                    // Arguments are 8-bit
                    arg1 = (int8_t) buffer[offset];
                    arg2 = (int8_t) buffer[offset + 1];
                    offset += 2;
                }

                // If ARGS_ARE_XY_VALUES flag is set, treat as x and y displacement
                if (flags & 0x0002) {
                    // arg1, arg2 are x and y offsets
                } else {
                    // arg1, arg2 are point numbers
                }

                // Check for transformation flags (scaling/rotation)
                if (flags & 0x0008) {
                    // WE_HAVE_A_SCALE - Single scale value
                    uint16_t scale = getInt16(buffer, offset);
                    offset += 2;
                    // Apply scale
                } else if (flags & 0x0040) {
                    // WE_HAVE_AN_X_AND_Y_SCALE - Separate x, y scaling
                    uint16_t xscale = getInt16(buffer, offset);
                    uint16_t yscale = getInt16(buffer, offset + 2);
                    offset += 4;
                    // Apply xscale and yscale
                } else if (flags & 0x0080) {
                    // WE_HAVE_A_TWO_BY_TWO - 2x2 transformation matrix
                    uint16_t xscale = getInt16(buffer, offset);
                    uint16_t scale01 = getInt16(buffer, offset + 2);
                    uint16_t scale10 = getInt16(buffer, offset + 4);
                    uint16_t yscale = getInt16(buffer, offset + 6);
                    offset += 8;
                    // Apply 2x2 matrix transformation
                }

                // If more components flag is set, continue the loop
                more_components = flags & 0x0020;
            }
        }
    }
}
