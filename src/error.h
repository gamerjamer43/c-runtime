#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>

typedef enum {
    ERROR_OUT_OF_MEMORY,
    ERROR_INDEX_OUT_OF_BOUNDS,
    ERROR_TYPE_MISMATCH,
    ERROR_NULL_POINTER,
    ERROR_INVALID_ARGUMENT,
    ERROR_UNKNOWN
} ErrorType;

static inline void error(ErrorType type, char* msg) {
    char* title = NULL;

    switch (type) {
        case ERROR_OUT_OF_MEMORY: 
            title = "OutOfMemory";
            break;

        case ERROR_INDEX_OUT_OF_BOUNDS: 
            title = "OutOfBounds";
            break;

        case ERROR_TYPE_MISMATCH:
            title = "TypeMismatch";
            break;

        case ERROR_NULL_POINTER:
            title = "NullPointer";
            break;

        case ERROR_INVALID_ARGUMENT:
            title = "InvalidArg";
            break;

        case ERROR_UNKNOWN:
            title = "Unknown";
            break;
    }

    printf("\n%sError:\n%s", title, msg);
    exit(0);
}

#endif