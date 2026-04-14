/**
 * dynarr - the api for safe use of dynamic arrays
 * contains inline functions for array resizing, appending, indexing, and creation
 * 
 * bool da_resize(DynArr *arr, usize new_cap)           -> returns true if a resize was possible (and done), false if something went wrong
 * bool da_append(DynArr *arr, Value value)             -> returns true if a value was added to the provided array, return false if something went wrong
 * Option da_index(const DynArr *arr, usize index) -> try and index into the array, if a value is presented return an Option with some = true, else return an option with some = false
 * DynArr da_make_arr(Type t)                           -> safely create a new dynamic array given its type
 * 
 * @author Noah Mingolelli
 */

#ifndef DYNARR_H
#define DYNARR_H

#include "typing.h"

/**
 * helper to create an array safely
 */
static inline DynArr da_make_arr(Type t) {
    return (DynArr){.type = t, .cap = 0, .len = 0, .data = NULL};
}

/**
 * attempt to resize the array provided a new size
 */
static inline bool da_resize(DynArr *arr, usize new_cap) {
    if (new_cap == arr->cap) return true;

    void *new_data = realloc(arr->data, new_cap * sizeof(Value));
    if (new_data == NULL && new_cap != 0) return false;

    arr->data = new_data;
    arr->cap = new_cap;
    if (arr->len > new_cap) arr->len = new_cap;

    return true;
}

/**
 * attempt to append a value to the end of the array, return boolean success 
 */
static inline bool da_append(DynArr *arr, Value value) {
    if (arr->type != value.type) return false;
    if (arr->len == arr->cap) {
        usize new_cap = arr->cap == 0 ? 8 : arr->cap * 2;

        if (!da_resize(arr, new_cap)) return false;
    }

    Value *data = (Value *)arr->data;
    data[arr->len++] = value;
    return true;
}

/**
 * index safely into an array, returning an option containing some if data is present, otherwise none
 */
static inline Option da_index(const DynArr *arr, usize index) {
    if (index >= arr->len) {
        return (Option){ 
            .some = false, 
            .val = make_val(TYPE_NULL, (TypedValue){.i = 0})
        };
    }

    Value *data = (Value *)arr->data;
    return (Option){ 
        .some = true,
        .val = data[index]
    };
}

/**
 * slot an item at a certain index of an array
 */
static inline bool da_insert(DynArr *arr, Value value, usize index) {
    if (!arr) return false;
    if (arr->type != value.type) return false;

    while (index >= arr->cap) {
        usize new_cap = arr->cap == 0 ? 8 : arr->cap * 2;

        if (!da_resize(arr, new_cap)) return false;
    }

    Value *data = (Value *)arr->data;
    
    // zero initalize all slots in between
    for (usize i = arr->len; i < index; i++) {
        data[i] = make_val(TYPE_NULL, (TypedValue){.i = 0});
    }

    arr->len = (index >= arr->len) ? index + 1 : arr->len;
    data[index] = value;
    return true;
}

/**
 * helper to recursively free flat and pointer based items in an array
 */
static inline bool da_free(DynArr* arr) {
    // freed already
    if (arr->data == NULL) return true;

    // nested array types (char array and array arrays)
    if (arr->type == TYPE_STR || arr->type == TYPE_ARR) {
        Value *data = (Value *)arr->data;

        for (usize i = 0; i < arr->len; i++) {
            // Null check before freeing nested types to handle gaps and uninitialized data
            if (arr->type == TYPE_STR && data[i].as.str != NULL) {
                free(data[i].as.str);
            } 
            
            else if (arr->type == TYPE_ARR && data[i].as.arr != NULL) {
                da_free(data[i].as.arr);
                free(data[i].as.arr);
            }
        }
    }

    // free container and null
    free(arr->data);
    arr->data = NULL;
    arr->len = 0;
    arr->cap = 0;

    // success
    return true;
}

/**
 * helper to slice sections of a dynamic array.
 * to iterate, simply use a for loop from start to end
 */
static inline Slice da_slice(DynArr* data, usize start, usize end) {
    if (data == NULL || start > end || end > data->len) return (Slice){0};
    return (Slice){.type = data->type, .data = data->data, .start = start, .end = end};
}

#endif