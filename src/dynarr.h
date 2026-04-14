/**
 * dynarr - the api for safe use of dynamic arrays
 * contains inline functions for array resizing, appending, indexing, and creation
 * 
 * bool da_resize(DynArr *arr, usize new_cap)           -> returns true if a resize was possible (and done), false if something went wrong
 * bool da_append(DynArr *arr, Value value)             -> returns true if a value was added to the provided array, return false if something went wrong
 * Option da_safe_index(const DynArr *arr, usize index) -> try and index into the array, if a value is presented return an Option with some = true, else return an option with some = false
 * DynArr da_make_arr(Type t)                           -> safely create a new dynamic array given its type
 * 
 * @author Noah Mingolelli
 */

#ifndef DYNARR_H
#define DYNARR_H

#include "typing.h"

/**
 * attempt to resize the array provided a new size
 */
static inline bool da_resize(DynArr *arr, usize new_cap) {
    void *new_data = realloc(arr->data, new_cap * sizeof(Value));
    if (new_data == NULL) return false;

    arr->data = new_data;
    arr->cap = new_cap;
    return true;
}

/**
 * attempt to append a value to the array, return boolean success 
 */
static inline bool da_append(DynArr *arr, Value value) {
    if (arr->len == arr->cap) {
        usize new_cap = arr->cap == 0 ? 8 : arr->cap * 2;

        if (!da_resize(arr, new_cap)) return false;
        if (arr->type != value.type) return false;
    }

    Value *data = (Value *)arr->data;
    data[arr->len++] = value;
    return true;
}

/**
 * index safely into an array, returning an option containing some if data is present, otherwise none
 */
static inline Option da_safe_index(const DynArr *arr, usize index) {
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
 * helper to create an array safely
 */
static inline DynArr da_make_arr(Type t) {
    return (DynArr){.type = t, .cap = 0, .len = 0, .data = NULL};
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
            // strings are null terminated and we free whats at the pointer
            if (arr->type == TYPE_STR) {
                free(data[i].val.str);
            } 
            
            // nested arrays need to have this called on them again
            else if (arr->type == TYPE_ARR) {
                da_free(data[i].val.arr);
                free(data[i].val.arr);
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

#endif