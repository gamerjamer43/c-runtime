#ifndef DYNARR_H
#define DYNARR_H

#include "typing.h"

/**
 * attempt to resize the array provided a new size
 */
static inline bool da_resize(DynArr *nums, usize new_cap) {
    void *new_data = realloc(nums->data, new_cap * sizeof(Value));
    if (new_data == NULL) return false;

    nums->data = new_data;
    nums->cap = new_cap;
    return true;
}

/**
 * attempt to append a value to the array, return boolean success 
 */
static inline bool da_append(DynArr *nums, Value value) {
    if (nums->len == nums->cap) {
        usize new_cap = nums->cap == 0 ? 8 : nums->cap * 2;

        if (!da_resize(nums, new_cap)) return false;
        if (nums->type != value.type) return false;
    }

    Value *arr = (Value *)nums->data;
    arr[nums->len++] = value;
    return true;
}

/**
 * index safely into an array, returning an option containing some if data is present, otherwise none
 */
static inline Option da_safe_index(const DynArr *nums, usize index) {
    if (index >= nums->len) {
        return (Option){ 
            .some = false, 
            .type = TYPE_NULL, 
            .val = 0
        };
    }

    Value *arr = (Value *)nums->data;
    return (Option){ 
        .some = true, 
        .type = nums->type,
        .val = (void*)(uintptr_t)arr[index].val.i
    };
}

/**
 * helper to create an array safely
 */
static inline DynArr da_make_arr(Type t) {
    return (DynArr){.type = t, .cap = 0, .len = 0, .data = NULL};
}

#endif