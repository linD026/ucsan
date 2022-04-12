#ifndef __ARCH_COMPILER__
#define __ARCH_COMPILER__

#if __STDC_VERSION__ > 201112L
#include <stdatomic.h>

#define barrier() atomic_thread_fence(memory_order_relaxed)

#elif defined(__GNUC__)

#define barrier() __atomic_thread_fence(__ATOMIC_RELAXED)

#else /* non-support */

#define barrier()

#endif

#endif /* __ARCH_COMPILER__ */
