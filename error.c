#ifndef SLANG_ERROR_H
#define SLANG_ERROR_H

#include <stdarg.h>
#include <string.h>
#include <libgen.h>

#define error(st) "\033[0;31m" st "\033[0m"
#define file(st) "\033[0;34m" st "\033[0m"

static void print_error(const char* filename, size_t lineno, size_t col, const char* fmt, ...) {
    va_list args;

    fprintf(stderr, file("%s %zd:%zd") " :: " error("error") ": ", basename((char*) filename), lineno, col);

    va_start(args, fmt);
    fprintf(stderr, fmt, args);
    va_end(args);
    fprintf(stderr, "\n");
}

static void print_line_with_pointer(const char *line, size_t line_no, size_t col_from, size_t col_to) {
    size_t sz = 0, i;
    char *ch;
    int padding;

    col_from--;
    col_to--;


    ch = strchr(line, '\n');

    if (ch) { 
        sz = (size_t) (ch - line);
    } else {
        sz = strlen(line);
    }

    padding = fprintf(stderr, "%8zd", line_no);
    fprintf(stderr, " | ");

    fprintf(stderr, "%*.*s", (int) col_from, (int) col_from, line);
    fprintf(stderr, error("%*.*s"), 
            (int) (col_to - col_from + 1), 
            (int) (col_to - col_from + 1), 
            line + col_from);
    fprintf(stderr, "%*.*s\n",
            (int) (sz - col_from - 1),
            (int) (sz - col_from - 1),
            line + col_to + 1);


    for (i = 0; i < padding; ++i) {
        fprintf(stderr, " ");
    }

    fprintf(stderr, " | ");

    for (i = 0; i < sz; ++i) {
        if (i == col_from) {
            fprintf(stderr, error("^"));
        } else if (i + 1 > col_from && i + 1 <= col_to) {
            fprintf(stderr, error("-"));
        } else {
            fprintf(stderr, " ");
        }
    }

    fprintf(stderr, "\n");

}

#endif // SLANG_ERROR_H
