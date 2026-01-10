#ifndef SLANG_DATA_H
#define SLANG_DATA_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef struct {
    size_t len;
    const char* str;
} String;

String from_cstring(const char* str);
String from_slice(const char* str, size_t st, size_t ed);

#ifdef SLANG_DATA_C

String from_cstring(const char* str) {
    size_t len = strlen(str);

    char* buff = malloc(len + 1);

    strcpy(buff, str);

    return (String) {
        .len = len,
        .str = buff
    };
}

String from_slice(const char* str, size_t st, size_t ed) {
    size_t len = ed - st + 1;

    char* buff = malloc(len + 1);

    strncpy(buff, str + st, len);
    buff[len] = '\0';

    return (String) {
        .len = len,
        .str = buff
    };
}

#endif // SLANG_DATA_C
#endif // SLANG_DATA_H
