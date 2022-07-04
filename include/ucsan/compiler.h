#ifndef __COMPILER__
#define __COMPILER__

#if defined(CONFIG_GCC)

#include <gcc-generic.h>

#else

#error "non-support"

#endif /* compiler config */

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

#ifndef __alias
#define __alias(symbol) __attribute__((__alias__(#symbol)))
#endif

#ifndef _RET_IP__
#define _RET_IP_ (unsigned long)__builtin_return_address(0)
#endif

#ifndef __always_inline
#define __always_inline inline __attribute__((__always_inline__))
#endif

#ifndef noinline
#define noinline __attribute__((__noinline__))
#endif

#endif /* __COMPILER__ */
