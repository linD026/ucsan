#ifndef __PER_CPU__

#include <sched.h>

void *__per_cpu(void *p, size_t ele_size);
#define per_cpu(v) ({ (typeof((v)) *)__per_cpu((void *)&(v), sizeof((v))); })

#endif /* __PER_CPU__ */
