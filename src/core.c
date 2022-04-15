#define _GNU_SOURCE
#include <ucsan/per_cpu.h>
#include <ucsan/compiler.h>
#include <uapi/ucsan.h>
#include <stdio.h>

DEFINE_PER_CPU(int, arr);
DEFINE_PER_CPU(int, arrb);

int test_per_cpu(void)
{
    int *per_cpu_p;

    per_cpu_p = per_cpu(arr);

    *per_cpu_p = 1234;

    barrier();

    printf("per cpu: %d %d\n", *per_cpu(arr), sched_getcpu());

    return 0;
}
