#include "ttf.h"
#include <stdio.h>
#include <stdlib.h>

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
            printf("%02x ", buffer[i]);
        }
        printf("\n"); // Print a newline for better formatting
    }

    fclose(fp);
    free(buffer);
}
