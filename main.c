#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "typing.h"
#include "dynarr.h"

int main() {
    DynArr nums = da_make_arr(TYPE_INT);

    // append items to the array
    for (int i = 0; i < 10; i++) {
        Value add = make_val(TYPE_INT, (TypedValue){.i = i});

        if (!da_append(&nums, add)) {
            fprintf(stderr, "Failed to append %d to array.\n", i);
            goto cleanup;
        }
    }

    // print safely (falls through to cleanup when done)
    for (usize i = 0; i < nums.len; i++) {
        Option opt = da_safe_index(&nums, i);
        if (opt.some) {
            printf("%lld\n", opt.val.val.i);
        }
    }

// using go to because clean! clean!
cleanup:
    free(nums.data);
    return EXIT_SUCCESS;
}