#include "http_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

unsigned char *read_image_file(const char *filename, size_t *size) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        printf("[FILE] Could not open file: %s\n", filename);
        return NULL;
    }
    
    // Get file size
    fseek(file, 0, SEEK_END);
    *size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // Allocate memory
    unsigned char *data = malloc(*size);
    if (!data) {
        fclose(file);
        return NULL;
    }
    
    // Read file
    size_t bytes_read = fread(data, 1, *size, file);
    fclose(file);
    
    if (bytes_read != *size) {
        free(data);
        return NULL;
    }
    
    return data;
}

void url_decode(char *str) {
    char *read = str;
    char *write = str;
    
    while (*read) {
        if (*read == '%' && isxdigit(read[1]) && isxdigit(read[2])) {
            // Decode %XX
            char hex[3] = {read[1], read[2], '\0'};
            *write = (char)strtol(hex, NULL, 16);
            read += 3;
            write++;
        } else if (*read == '+') {
            // + becomes space
            *write = ' ';
            read++;
            write++;
        } else {
            *write = *read;
            read++;
            write++;
        }
    }
    *write = '\0';
}

const char *get_mime_type(const char *filename) {
    const char *ext = strrchr(filename, '.');
    if (!ext) return "application/octet-stream";
    
    if (strcmp(ext, ".html") == 0 || strcmp(ext, ".htm") == 0) {
        return "text/html";
    } else if (strcmp(ext, ".css") == 0) {
        return "text/css";
    } else if (strcmp(ext, ".js") == 0) {
        return "application/javascript";
    } else if (strcmp(ext, ".json") == 0) {
        return "application/json";
    } else if (strcmp(ext, ".png") == 0) {
        return "image/png";
    } else if (strcmp(ext, ".jpg") == 0 || strcmp(ext, ".jpeg") == 0) {
        return "image/jpeg";
    } else if (strcmp(ext, ".gif") == 0) {
        return "image/gif";
    } else if (strcmp(ext, ".svg") == 0) {
        return "image/svg+xml";
    } else if (strcmp(ext, ".txt") == 0) {
        return "text/plain";
    } else if (strcmp(ext, ".pdf") == 0) {
        return "application/pdf";
    }
    
    return "application/octet-stream";
}