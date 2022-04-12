#ifndef __COMPILER__
#define __COMPILER__

#if __STDC_VERSION__ > 201112L

#include <stdatomic.h>

#define smp_mb() atomic_thread_fence(memory_order_seq_cst)
#define smp_rmb() atomic_thread_fence(memory_order_acquire)
#define smp_wmb() atomic_thread_fence(memory_order_release)

#elif defined(__GNUC__)

#define smp_mb() __atomic_thread_fence(__ATOMIC_SEQ_CST)
#define smp_rmb() __atomic_thread_fence(__ATOMIC_ACQUIRE)
#define smp_wmb() __atomic_thread_fence(__ATOMIC_RELEASE)

#else

#error "non-support"

#endif /* compiler and stdc version */

/* x86 specific */
#define COHERENCE_SIZE 128

#ifndef barrier
#define barrier() asm volatile("" : : : "memory")
#endif

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#ifndef container_of
#define container_of(ptr, type, member)                    \
    ({                                                     \
        const typeof(((type *)0)->member) *__mptr = (ptr); \
        (type *)((char *)__mptr - offsetof(type, member)); \
    })
#endif

#endif /* __COMPILER__ */
