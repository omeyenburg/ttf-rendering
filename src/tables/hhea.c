#include "../ttf.h"

uint16_t parse_hhea(unsigned char* buffer, Table* hhea) {
    if (!hhea->initialized) {
        fprintf(stderr, "Table \"hhea\" was not found!\n");
        exit(1);
    }

    if (!validateCheckSum(buffer, hhea, 0)) {
        fprintf(stderr, "Invalid check sum for table \"hhea\"!\n");
        exit(1);
    }

    // Check version
    uint16_t major_version = getUInt16(buffer, hhea->offset);
    if (major_version != 1) {
        fprintf(stderr, "Unexpected major version in hhea table: %i\n", major_version);
        exit(1);
    }

    uint16_t minor_version = getUInt16(buffer, hhea->offset + 2);
    if (minor_version != 0) {
        fprintf(stderr, "Unexpected minor version in hhea table: %i\n", minor_version);
        exit(1);
    }

    // Read number of hMetric entries in hmtx table
    uint16_t numberOfHMetrics = getUInt16(buffer, hhea->offset + 34);
    return numberOfHMetrics;
}
