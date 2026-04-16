#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "src/typing.h"
#include "src/dynarr.h"
#include "src/error.h"

int main() {
    DynArr nums = da_make_arr(TYPE_INT);

    printf("Append 10 items to the array\n");
    for (i32 i = 0; i < 10; i++) {
        Value add = AS_VAL(i);

        if (!da_append(&nums, add)) {
            fprintf(stderr, "Failed to append %d to array.\n", i);
            goto cleanup;
        }
    }

    printf("Print those 10 items from the array:\n");
    for (usize i = 0; i < nums.len; i++) {
        Option opt = da_index(&nums, i);
        Value unwrapped = unwrap_option(opt);

        if (unwrapped.type == TYPE_INT) {
            printf("%lld\n", unwrapped.as.i);
        }
    }

    printf("\nInserted 42 at nums[0]\n");
    Value val = AS_VAL(42);
    da_insert(&nums, val, 0);

    printf("Confirm 42 was swapped:\n");
    val = unwrap_option(da_index(&nums, 0));
    printf("%lli\n", val.as.i);

    printf("\nCreate a string value:\n");
    val = as_str("hi");
    printf("%s\n", val.as.str);

    printf("\nSlice from 0 to 2\n");
    Slice slice = da_slice(&nums, 0, 3);
    Value* items = (Value*)slice.data;

    printf("Print that slice:\n");
    for (usize i = slice.start; i < slice.end; i++) {
        printf("%lli\n", items[i].as.i);
    }

    printf("\nTesting casting. Created a signed integer 42\n");
    val = AS_VAL(42);

    printf("Casting to unsigned int\n");
    if (!try_cast(TYPE_UINT, val, &val)) {
        fprintf(stderr, "cast failed\n");
    } else {
        printf("cast ok: %llu\n", (unsigned long long)val.as.u);
    }

// using go to because clean! clean!
cleanup:
    da_free(&nums);
    return EXIT_SUCCESS;
}