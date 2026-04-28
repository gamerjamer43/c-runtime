#ifndef MAP_H
#define MAP_H

#include "typing.h"

/**
 * owned pair of two items, containing a type
 */
typedef struct {
    void* one;
    void* two;
    Type type;

    // explicit padding
    u8 padding[7];
} Pair;

/**
 * owned pair of two items, containing a type for EACH
 * (as they can have different types)
 */
typedef struct {
    // i can figure out better names trust
    void* key;
    void* val;

    Type key_type;
    Type val_type;

    // explicit padding
    u8 padding[6];
} KVPair;

/**
 * borrowed n-tuple of items. contains a pointer to each item
 * which can be incremented through like normal. use a safe cast
 * to cast the pointer into its proper type before operating on the data inside
 */
typedef struct {
    void* data;
    u32 count;
    Type type;

    // explicit padding
    u8 padding[3];
} Tuple;


/**
 * map of key value pairs. the underlying values array is borrowed.
 * must be allocated and resized using the safe api
 */
typedef struct {
    KVPair* values;
    u32 len;
    u32 cap;
} Map;

#endif