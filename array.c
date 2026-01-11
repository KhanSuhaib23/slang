/*
 * Just include this file inside any code that needs to use it, like:
 * #include "array.c"
 *
 * Usage:
 * Define a new array for a particular type
 *
 * typedef Array(Your_Type) Your_Type_Array; // give it any name you want
 *
 * Initialize a new array
 *
 * Your_Type_Array arr = (Your_Type_Array) {0};
 *
 * Push elements to the array
 *
 * array_push_back(&arr, elem1);
 * array_push_back(&arr, elem2);
 *
 * Iterate through the array
 *
 * for (size_t i = 0; i < arr.sz; ++i) {
 *     Your_Type it = arr.arr[i];
 *     // use it
 * }
 *
 */


#ifndef ARRAY_H
#define ARRAY_H

#include <stdio.h>
#include <stdlib.h>

#define Array(T) struct { \
    size_t cp, sz; \
    T* arr; \
}

#define array_push_back(array, element) (__array_maybe_grow((Array*) array, sizeof(element)), \
        ((array)->arr[(array)->sz++]) = (element))

typedef Array(void) Array;

static void __array_maybe_grow(Array* array, size_t element_size) {
    if (array->sz == array->cp) {
        array->cp = array->cp * 2 + 1;
        array->arr = realloc(array->arr, array->cp * element_size);
    }
}

#endif // ARRAY_H
