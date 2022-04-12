#ifndef __ARCH_COMPILER__
#define __ARCH_COMPILER__

#define barrier() asm volatile("" : : : "memory")

#endif /* __ARCH_COMPILER__ */
