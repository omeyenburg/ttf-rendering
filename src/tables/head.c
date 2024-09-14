#include "../ttf.h"

int16_t parse_head(unsigned char* buffer, Table* head) {
    if (!head->initialized) {
        fprintf(stderr, "Table \"head\" was not found!\n");
        exit(1);
    }

    uint32_t checkSumAdjustment = getInt32(buffer, head->offset + 8);
    if (!validateCheckSum(buffer, head, checkSumAdjustment)) {
        fprintf(stderr, "Invalid check sum for table \"head\"!\n");
        exit(1);
    }

    // Read indexToLocFormat for loca table
    // 0 => short offset; 1 => long offset
    int16_t indexToLocFormat = getInt16(buffer, head->offset + 50);
    if (indexToLocFormat != 0 && indexToLocFormat != 1) {
        fprintf(
            stderr, "Invalid indexToLocFormat %i, expected 0 or 1.", indexToLocFormat);
        exit(1);
    }

    return indexToLocFormat;
}
