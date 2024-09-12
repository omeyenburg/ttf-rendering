#include "ttf.h"

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
            // printf("%02x ", buffer[i]);
            // printf("%c ", c);
        }
        printf("\n"); // Print a newline for better formatting
    }

    uint32_t version = bytes4xint(buffer, 0);
    printf("Version: %i\n", version);

    uint16_t num_tables = bytes2xint(buffer, 4);
    printf("Num tables: %i\n", num_tables);

    struct Table cmap;
    struct Table glyf;
    struct Table head;
    struct Table hhea;
    struct Table htmx;
    struct Table loca;
    struct Table maxp;
    struct Table name;
    struct Table post;

    int buffer_offset = 12;
    for (int i = 0; i < num_tables; i++) {
        const char* label = bytes4xchar(buffer, buffer_offset);
        uint32_t check_sum = bytes4xint(buffer, buffer_offset + 4);
        uint32_t offset = bytes4xint(buffer, buffer_offset + 8);
        uint32_t length = bytes4xint(buffer, buffer_offset + 12);
        printf("Label: %s\t", label);
        printf("Check sum: %u\t", check_sum);
        printf("Offset: %u\t", offset);
        printf("Length: %u\n", length);

        if (strcmp(label, "cmap") == 0) {
            cmap.initialized = true;
            cmap.check_sum = check_sum;
            cmap.offset = offset;
            cmap.length = length;
        } else if (strcmp(label, "glyf") == 0) {
            glyf.initialized = true;
            glyf.check_sum = check_sum;
            glyf.offset = offset;
            glyf.length = length;
        } else if (strcmp(label, "head") == 0) {
            head.initialized = true;
            head.check_sum = check_sum;
            head.offset = offset;
            head.length = length;
        } else if (strcmp(label, "hhea") == 0) {
            hhea.initialized = true;
            hhea.check_sum = check_sum;
            hhea.offset = offset;
            hhea.length = length;
        } else if (strcmp(label, "htmx") == 0) {
            htmx.initialized = true;
            htmx.check_sum = check_sum;
            htmx.offset = offset;
            htmx.length = length;
        } else if (strcmp(label, "loca") == 0) {
            loca.initialized = true;
            loca.check_sum = check_sum;
            loca.offset = offset;
            loca.length = length;
        } else if (strcmp(label, "maxp") == 0) {
            maxp.initialized = true;
            maxp.check_sum = check_sum;
            maxp.offset = offset;
            maxp.length = length;
        } else if (strcmp(label, "name") == 0) {
            name.initialized = true;
            name.check_sum = check_sum;
            name.offset = offset;
            name.length = length;
        } else if (strcmp(label, "post") == 0) {
            post.initialized = true;
            post.check_sum = check_sum;
            post.offset = offset;
            post.length = length;
        }

        buffer_offset += 16;
    }

    fclose(fp);
    free(buffer);

    if (!cmap.initialized) {
        fprintf(stderr, "Table \"cmap\" was not found!\n");
        exit(1);
    } else if (!glyf.initialized) {
        fprintf(stderr, "Table \"glyf\" was not found!:\n");
        exit(1);
    } else if (!head.initialized) {
        fprintf(stderr, "Table \"head\" was not found!:\n");
        exit(1);
    } else if (!hhea.initialized) {
        fprintf(stderr, "Table \"hhea\" was not found!:\n");
        exit(1);
    } else if (!htmx.initialized) {
        fprintf(stderr, "Table \"htmx\" was not found!:\n");
        exit(1);
    } else if (!loca.initialized) {
        fprintf(stderr, "Table \"loca\" was not found!:\n");
        exit(1);
    } else if (!maxp.initialized) {
        fprintf(stderr, "Table \"maxp\" was not found!:\n");
        exit(1);
    } else if (!name.initialized) {
        fprintf(stderr, "Table \"name\" was not found!:\n");
        exit(1);
    } else if (!post.initialized) {
        fprintf(stderr, "Table \"post\" was not found!:\n");
        exit(1);
    }
}
