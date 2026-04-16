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

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    if (!opt.some) return AS_VAL(0);
    return opt.val;
}

/**
 * safe helper to cast between types
 */
static inline bool try_cast(Type target, Value in, Value* out) {
    // STEP 1: validate pointers and enum inputs
    if (out == NULL) return false;

    if (target < 0 || target >= COUNT_OF_TYPES || in.type < 0 || in.type >= COUNT_OF_TYPES) {
        *out = make_val(TYPE_NULL, (TypedValue){.i = 0});
        return false;
    }

    // STEP 2: fast path for same-type casts
    if (target == in.type) {
        *out = in;
        return true;
    }

    Value result = make_val(TYPE_NULL, (TypedValue){.i = 0});
    bool success = false;

    // STEP 3: switch on source type, then handle allowed targets
    switch (in.type) {
        case TYPE_INT: {
            // allow int -> uint/size/float/double/char/bool with range checks
            const i64 v = in.as.i;

            switch (target) {
                case TYPE_BOOL:
                    result = make_val(TYPE_BOOL, (TypedValue){.b = (v != 0)});
                    success = true;
                    break;

                case TYPE_CHAR:
                    if (v < CHAR_MIN || v > CHAR_MAX) break;
                    result = make_val(TYPE_CHAR, (TypedValue){.c = (char)v});
                    success = true;
                    break;

                case TYPE_UINT:
                    if (v < 0) break;
                    result = make_val(TYPE_UINT, (TypedValue){.u = (u64)v});
                    success = true;
                    break;

                case TYPE_SIZE:
                    if (v < 0 || (u64)v > (u64)SIZE_MAX) break;
                    result = make_val(TYPE_SIZE, (TypedValue){.s = (usize)v});
                    success = true;
                    break;

                case TYPE_FLOAT:
                    result = make_val(TYPE_FLOAT, (TypedValue){.f = (f32)v});
                    success = true;
                    break;

                case TYPE_DOUBLE:
                    result = make_val(TYPE_DOUBLE, (TypedValue){.d = (f64)v});
                    success = true;
                    break;

                default:
                    break;
            }
        }

        case TYPE_UINT:
        case TYPE_SIZE: {
            // allow to signed/float/double/char/bool with range checks
            const u64 v = (in.type == TYPE_UINT) ? in.as.u : (u64)in.as.s;

            switch (target) {
                case TYPE_BOOL:
                    result = make_val(TYPE_BOOL, (TypedValue){.b = (v != 0)});
                    success = true;
                    break;

                case TYPE_CHAR:
                    if (v > (u64)CHAR_MAX) break;
                    result = make_val(TYPE_CHAR, (TypedValue){.c = (char)v});
                    success = true;
                    break;

                case TYPE_INT:
                    if (v > (u64)LLONG_MAX) break;
                    result = make_val(TYPE_INT, (TypedValue){.i = (i64)v});
                    success = true;
                    break;

                case TYPE_UINT:
                    if (in.type == TYPE_SIZE && sizeof(usize) > sizeof(u64) && in.as.s > (usize)UINT64_MAX) break;
                    result = make_val(TYPE_UINT, (TypedValue){.u = v});
                    success = true;
                    break;

                case TYPE_SIZE:
                    if (in.type == TYPE_UINT && v > (u64)SIZE_MAX) break;
                    result = make_val(TYPE_SIZE, (TypedValue){.s = (usize)v});
                    success = true;
                    break;

                case TYPE_FLOAT:
                    result = make_val(TYPE_FLOAT, (TypedValue){.f = (f32)v});
                    success = true;
                    break;

                case TYPE_DOUBLE:
                    result = make_val(TYPE_DOUBLE, (TypedValue){.d = (f64)v});
                    success = true;
                    break;

                default:
                    break;
            }
        }

        case TYPE_FLOAT:
        case TYPE_DOUBLE: {
            // reject NaN/Inf for integer targets
            const f64 v = (in.type == TYPE_FLOAT) ? (f64)in.as.f : in.as.d;

            switch (target) {
                case TYPE_BOOL:
                    result = make_val(TYPE_BOOL, (TypedValue){.b = (v != 0.0)});
                    success = true;
                    break;

                case TYPE_CHAR:
                    if (!isfinite(v) || trunc(v) != v || v < (f64)CHAR_MIN || v > (f64)CHAR_MAX) break;
                    result = make_val(TYPE_CHAR, (TypedValue){.c = (char)v});
                    success = true;
                    break;

                case TYPE_INT:
                    if (!isfinite(v) || trunc(v) != v || v < (f64)LLONG_MIN || v > (f64)LLONG_MAX) break;
                    result = make_val(TYPE_INT, (TypedValue){.i = (i64)v});
                    success = true;
                    break;

                case TYPE_UINT:
                    if (!isfinite(v) || trunc(v) != v || v < 0.0 || v > (f64)ULLONG_MAX) break;
                    result = make_val(TYPE_UINT, (TypedValue){.u = (u64)v});
                    success = true;
                    break;

                case TYPE_SIZE:
                    if (!isfinite(v) || trunc(v) != v || v < 0.0 || v > (f64)SIZE_MAX) break;
                    result = make_val(TYPE_SIZE, (TypedValue){.s = (usize)v});
                    success = true;
                    break;

                case TYPE_FLOAT:
                    if (isfinite(v) && fabs(v) > (f64)FLT_MAX) break;
                    result = make_val(TYPE_FLOAT, (TypedValue){.f = (f32)v});
                    success = true;
                    break;

                case TYPE_DOUBLE:
                    result = make_val(TYPE_DOUBLE, (TypedValue){.d = (f64)v});
                    success = true;
                    break;

                default:
                    break;
            }
        }

        case TYPE_CHAR:
        case TYPE_BOOL: {
            // - allow to numeric targets (simple widening rules)
            const i64 v = (in.type == TYPE_CHAR) ? (i64)in.as.c : (in.as.b ? 1 : 0);

            switch (target) {
                case TYPE_BOOL:
                    result = make_val(TYPE_BOOL, (TypedValue){.b = (v != 0)});
                    success = true;
                    break;

                case TYPE_CHAR:
                    if (v < CHAR_MIN || v > CHAR_MAX) break;
                    result = make_val(TYPE_CHAR, (TypedValue){.c = (char)v});
                    success = true;
                    break;

                case TYPE_INT:
                    result = make_val(TYPE_INT, (TypedValue){.i = v});
                    success = true;
                    break;

                case TYPE_UINT:
                    if (v < 0) break;
                    result = make_val(TYPE_UINT, (TypedValue){.u = (u64)v});
                    success = true;
                    break;

                case TYPE_SIZE:
                    if (v < 0) break;
                    result = make_val(TYPE_SIZE, (TypedValue){.s = (usize)v});
                    success = true;
                    break;

                case TYPE_FLOAT:
                    result = make_val(TYPE_FLOAT, (TypedValue){.f = (f32)v});
                    success = true;
                    break;

                case TYPE_DOUBLE:
                    result = make_val(TYPE_DOUBLE, (TypedValue){.d = (f64)v});
                    success = true;
                    break;

                default:
                    break;
            }
        }

        case TYPE_STR: {
            // parse numeric values from string safely
            break;
        }

        // don't know what to do for this yet
        case TYPE_ARR: break;

        // null only casts to itself (handled in the fast path)
        case TYPE_NULL: break;

        // reject as invalid type
        case COUNT_OF_TYPES: break;
    }

    // STEP 4: if conversion path succeeded, write *out and return true
    // STEP 5: on any failure, write a null and return false
    if (success) {
        *out = result;
        return true;
    }

    *out = make_val(TYPE_NULL, (TypedValue){.i = 0});
    return false;
}

#endif