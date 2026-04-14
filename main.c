#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "src/typing.h"
#include "src/dynarr.h"

int main() {
    DynArr nums = da_make_arr(TYPE_INT);

    // append items to the array
    for (i32 i = 0; i < 10; i++) {
        Value add = AS_VAL(i);

        if (!da_append(&nums, add)) {
            fprintf(stderr, "Failed to append %d to array.\n", i);
            goto cleanup;
        }
    }

    // print safely (falls through to cleanup when done)
    for (usize i = 0; i < nums.len; i++) {
        Option opt = da_index(&nums, i);
        Value unwrapped = unwrap_option(opt);

        if (unwrapped.type != TYPE_NULL) {
            printf("%lld\n", unwrapped.as.i);
        }
    }

    // insert 42 at position 0
    Value val = AS_VAL(42);
    da_insert(&nums, val, 0);

    // print that out
    val = unwrap_option(da_index(&nums, 0));
    printf("%lli\n", val.as.i);

    val = as_str("hi");
    printf("%s\n", val.as.str);

// using go to because clean! clean!
cleanup:
    da_free(&nums);
    return EXIT_SUCCESS;
}