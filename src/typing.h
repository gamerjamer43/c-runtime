/**
 * typing - the formal spec for type management in this system.
 * holds the standard Type enum, the dynamic array type, typed values, options, 
 * along with a safe api for creation of both options and values
 * 
 * Type       -> enum containing all types recognized (with the last value reserved for sizing)
 * DynArr     -> struct containing a pointer to the array items (which can be pointer indexed) as well as type and size details
 * TypedValue -> a union containing all types recognized in type (null is an int value of 0)
 * Value      -> a struct containing a TypedValue and its Type (allowing us to pun properly)
 * 
 * char* type_of(Value value)                -> given a value, return the string type of it's name (RTTI)
 * Value make_val(Type type, TypedValue val) -> given a Type and TypedValue, safely validate and return a Value
 * Value unwrap_option(Option opt)           -> return the options value, or null if nothing is presented. considering adding a unit type for this
 * 
 * @author Noah Mingolelli
 */

#ifndef TYPING_H
#define TYPING_H

#include <stdlib.h>
#include "ntypes.h"

/**
 * macro helper to cleanly provide creation of values at comp time
 * char* doesnt work due to an implicit cast 
 */
#define AS_VAL(x) _Generic((x), \
    int:       make_val(TYPE_INT,    (TypedValue){.i =   (i64)(x)}), \
    long long: make_val(TYPE_INT,    (TypedValue){.i =   (i64)(x)}), \
    float:     make_val(TYPE_FLOAT,  (TypedValue){.f =   (f32)(x)}), \
    double:    make_val(TYPE_DOUBLE, (TypedValue){.d =   (f64)(x)}), \
    char:      make_val(TYPE_CHAR,   (TypedValue){.c =   (char)(x)}) \
)

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
 * dynamic arrays, resizeable with the api calls below
 */
typedef struct {
    void* data;
    usize cap;
    usize len;
    Type type;
} DynArr;

/**
 * union container that can hold any recognized type
 * chars and dynamic arrays are referenced rather than owned for a max 8 byte size
 */
typedef union {
    bool b;
    char c;
    i64 i;
    u64 u;
    usize s;
    f32 f;
    f64 d;
    char* str;
    DynArr* arr;
} TypedValue;

/**
 * something definitively containing a value
 */
typedef struct {
    TypedValue as;
    Type type;

    // explicit padding i might be able to do something with
    u8 padding[7];
} Value;

/**
 * optional value wrapper. 
 * if some is true, it can be unwrapped
 */
typedef struct {
    Value val;
    bool some;

    // explicit padding i might be able to do something with
    u8 padding[7];
} Option;

/**
 * array slice. fat pointer containing a type, and start to end range
 */
typedef struct {
    void* data;
    usize start;
    usize end;
    Type type;
} Slice;

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
static inline Value make_val(Type type, TypedValue val) {
    if (type < 0 || type >= COUNT_OF_TYPES) {
        return (Value){.type = TYPE_NULL, .as = (TypedValue){.i = 0}};
    }

    return (Value){.type = type, .as = val};
}

/**
 * because it wasnt working with char* here's an as_str helper too
 */
static inline Value as_str(char* data) {
    return make_val(TYPE_STR, (TypedValue){.str = data});
}

/**
 * safe helper to unwrap option types
 */
static inline Value unwrap_option(Option opt) {
    if (opt.some) {
        return opt.val;
    }

    return AS_VAL(0);
}

#endif