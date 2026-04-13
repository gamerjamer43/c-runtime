#ifndef TYPING_H
#define TYPING_H

#include <stdbool.h>
#include <stdint.h>
#include "ntypes.h"

/**
 * enum containing all types recognized by the runtime.
 */
typedef enum {
    TYPE_NULL,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_INT,
    TYPE_UINT,
    TYPE_SIZE,
    TYPE_FLOAT,
    TYPE_DOUBLE,
    TYPE_STR,
    TYPE_ARR,

    // last value of an enum contains the length :3
    COUNT_OF_TYPES
} Type;

/**
 * optional value wrapper. 
 * if some is true, it can be unwrapped
 */
typedef struct {
    void* val;
    bool some;
    Type type;
} Option;

/**
 * dynamic arrays, resizeable with the api calls below
 */
typedef struct {
    Type type;
    void* data;
    usize cap;
    usize len;
} DynArr;

/**
 * union container that can hold any recognized type
 * chars and dynamic arrays are referenced rather than owned for a max 8 byte size
 */
typedef union {
    bool b;
    char c;
    int64_t i;
    uint64_t u;
    usize s;
    float f;
    double d;
    char* str;
    DynArr* arr;
} TypedValue;

typedef struct {
    TypedValue val;
    Type type;
} Value;

/**
 * return the type of a provided value
 */
static inline const char* type_of(Value value) {
    switch (value.type) {
        case TYPE_NULL: return "null";
        case TYPE_BOOL: return "bool";
        case TYPE_CHAR: return "char";
        case TYPE_INT: return "int";
        case TYPE_UINT: return "uint";
        case TYPE_SIZE: return "size";
        case TYPE_FLOAT: return "float";
        case TYPE_DOUBLE: return "double";
        case TYPE_STR: return "str";
        case TYPE_ARR: return "arr";

        // i don't care fuck you
        case COUNT_OF_TYPES: printf("bruh...\n"); exit(0);
    }

    return "unknown";
}

/**
 * simple helper to make values safely
 */
static inline Value make_val(Type t, TypedValue v) {
    if (t < 0 || t >= COUNT_OF_TYPES) {
        return (Value){.type = TYPE_NULL, .val = (TypedValue){.i = 0}};
    }

    return (Value){.type = t, .val = v};
}

#endif