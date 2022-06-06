#ifndef CONFIG_GCC

#if defined(__GNUC__)

#define smp_mb() __atomic_thread_fence(__ATOMIC_SEQ_CST)
#define smp_rmb() __atomic_thread_fence(__ATOMIC_ACQUIRE)
#define smp_wmb() __atomic_thread_fence(__ATOMIC_RELEASE)

#else

#error "CONFIG_GCC set, but doesn't support it"

#endif

#endif /* CONFIG_GCC */
