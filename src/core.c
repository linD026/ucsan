#define _GNU_SOURCE
#include <stdio.h>
#include <ucsan/per_cpu.h>

int test_per_cpu(void)
{
    int arr[40] = { 0 };
    int *p, *per_cpu_p;
    int i;

    for (i = 0; i < 40; i++)
        arr[i] = i;

    p = arr;

    per_cpu_p = per_cpu(*p);

    printf("per cpu: %d %d\n", *per_cpu_p, sched_getcpu());

    return 0;
}
