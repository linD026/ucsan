#ifndef __PER_CPU__

#include <sched.h>

#define DEFINE_PER_CPU(type, name)     \
    struct __per_cpu_##type##_##name { \
        type __type;                   \
        type __per_cpu[UCSAN_NR_CPU];  \
    } name

void *__per_cpu(void *p, size_t ele_size);
#define per_cpu(v)                                                     \
    ({                                                                 \
        (typeof((&(v))->__type) *)__per_cpu((void *)(&(v))->__per_cpu, \
                                            sizeof((v)));              \
    })

#endif /* __PER_CPU__ */
