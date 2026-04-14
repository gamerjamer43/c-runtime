#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "src/typing.h"
#include "src/dynarr.h"

int main() {
    DynArr nums = da_make_arr(TYPE_INT);

    // append items to the array
    for (i32 i = 0; i < 10; i++) {
        Value add = make_val(TYPE_INT, (TypedValue){.i = i});

        if (!da_append(&nums, add)) {
            fprintf(stderr, "Failed to append %d to array.\n", i);
            goto cleanup;
        }
    }

    // print safely (falls through to cleanup when done)
    for (usize i = 0; i < nums.len; i++) {
        Option opt = da_safe_index(&nums, i);
        Value unwrapped = unwrap_option(opt);

        if (unwrapped.type != TYPE_NULL) {
            printf("%lld\n", unwrapped.as.i);
        }
    }

    // insert 42 at position 0
    Value val = make_val(TYPE_INT, (TypedValue){.i = 42});
    da_safe_insert(&nums, val, 0);

    // print that out
    val = unwrap_option(da_safe_index(&nums, 0));
    printf("%lli", val.as.i);

// using go to because clean! clean!
cleanup:
    da_free(&nums);
    return EXIT_SUCCESS;
}