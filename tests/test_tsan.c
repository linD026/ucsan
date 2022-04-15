/* gcc -c test_tsan.c
 * gcc -o test test_tsan.o test_ucsan.c -fsanitize=thread
 */

#include <stdio.h>
#include <stdint.h>

#include "../include/uapi/ucsan.h"

int main(void)
{
    int32_t i32v;
    
    printf("[BEFORE] i32v, val=%d, addr=%p\n", i32v, &i32v);

    i32v = 0;

    printf("[AFTER] i32v, val=%d, addr=%p\n", i32v, &i32v);

    tsan_read4((void *) &i32v);

    return 0;
}
