#ifndef __COMPILER__
#define __COMPILER__

#if defined(CONFIG_GCC)

#include <gcc-generic.h>

#else

#error "non-support"

#endif /* GCC or Other compiler config */

/* x86 specific */
#define COHERENCE_SIZE 128

#ifndef barrier
#define barrier() asm volatile("" : : : "memory")
#endif

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#ifndef container_of
#define container_of(ptr, type, member)                            \
	({                                                         \
		const typeof(((type *)0)->member) *__mptr = (ptr); \
		(type *)((char *)__mptr - offsetof(type, member)); \
	})
#endif

#define __alias(symbol) __attribute__((__alias__(#symbol)))
#define _RET_IP_ (unsigned long)__builtin_return_address(0)
#define __always_inline inline __attribute__((__always_inline__))

#endif /* __COMPILER__ */
