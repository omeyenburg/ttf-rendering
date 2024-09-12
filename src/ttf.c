#include "ttf.h"
#include <stdio.h>
#include <stdlib.h>

#define bytes4xint(buffer, offset)                                                     \
    (((unsigned char) buffer[offset] << 24) |                                          \
     ((unsigned char) buffer[offset + 1] << 16) |                                      \
     ((unsigned char) buffer[offset + 2] << 8) | ((unsigned char) buffer[offset + 3]))

#define bytes2xint(buffer, offset)                                                     \
    (((unsigned char) buffer[offset] << 8) | ((unsigned char) buffer[offset + 1]))

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
            path
        );
        free(buffer);
        fclose(fp);
        exit(1);
    } else {
        for (size_t i = 0; i < size; i++) {
            char c = buffer[i];
            printf("%02x ", buffer[i]);
            // printf("%c ", c);
        }
        printf("\n"); // Print a newline for better formatting
    }

    long version = bytes4xint(buffer, 0);
    printf("Version: %li\n", version);

    long num_tables = bytes2xint(buffer, 4);
    printf("Num tables: %li\n", num_tables);

    int offset = 12;
    for (int i = 0; i < num_tables; i++) {
        const char* label = bytes4xchar(buffer, offset);
        unsigned long check_sum = bytes4xint(buffer, offset + 4);
        unsigned long boffset = bytes4xint(buffer, offset + 8);
        unsigned long length = bytes4xint(buffer, offset + 12);
        printf("Label: %s\n", label);
        printf("Check sum: %lu\n", check_sum);
        printf("Offset: %lu\n", boffset);
        printf("Length: %lu\n", length);
        offset += 16;
    }

    fclose(fp);
    free(buffer);
}
