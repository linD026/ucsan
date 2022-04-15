#define _GNU_SOURCE
#include <sched.h>
#include <ucsan/per_cpu.h>

void *__per_cpu(void *p, size_t ele_size)
{
    char *start = (char *)p;
    int cpu = sched_getcpu();

    return (void *) start + (cpu * ele_size);
}
