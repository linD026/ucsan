#define _GNU_SOURCE
#include <ucsan/compiler.h>
#include <ucsan/encoding.h>
#include <ucsan/ucsan.h>
#include <ucsan/unify.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>

atomic_long watchpoints[NR_UCSAN_SLOT * NR_UCSAN_WP];

static __always_inline unsigned long
encode_watchpoint(unsigned long addr, size_t size, bool expect_write)
{
	unsigned long addr_masked = 0;

	/*
	 * First, we create the wp type of value from the paramters.
	 * We need to set up the write, size flags, and the addr mask.
	 */
	switch (size) {
	case 1:
		addr_masked = WATCHPOINT_SIZE_1;
		break;
	case 2:
		addr_masked = WATCHPOINT_SIZE_2;
		break;
	case 4:
		addr_masked = WATCHPOINT_SIZE_4;
		break;
	case 8:
		addr_masked = WATCHPOINT_SIZE_8;
		break;
	case 16:
		addr_masked = WATCHPOINT_SIZE_16;
		break;
	default:
		/* TODO: format the error report */
		printf("ERROR: Doesn't support the size\n");
		exit(0);
	}
	if (expect_write)
		addr_masked |= WATCHPOINT_WRITE_MASK;
	addr_masked |= addr & WATCHPOINT_ADDR_MASK;

	return addr_masked;
}

static inline atomic_long *insert_watchpoint(unsigned long addr, size_t size,
					     bool expect_write)
{
	const unsigned long slot = WP_SLOT(addr);
	unsigned long addr_masked = encode_watchpoint(addr, size, expect_write);
	atomic_long *watchpoint = NULL;
	long invalid_watchpoint = 0;
	unsigned int i;

	for (i = 0; i < NR_UCSAN_WP; i++) {
		/* We get the address of index first */
		watchpoint = &watchpoints[slot + i];
		/* if the watchpoint doesn't used, register it. */
		if (atomic_compare_exchange_strong(
			    watchpoint, &invalid_watchpoint, addr_masked))
			return watchpoint;
	}
	return NULL;
}

static __always_inline atomic_long *
find_watchpoint(unsigned long addr, size_t size, bool expect_write)
{
	const unsigned long slot = WP_SLOT(addr);
	unsigned long addr_masked = encode_watchpoint(addr, size, expect_write);
	atomic_long *watchpoint = NULL;
	int i;

	/*
	 * We set up the addr_masked, now we can travel the slots to find the
	 * corresponding watchpoint.
	 */
	for (i = 0; i < NR_UCSAN_WP; i++) {
		/* We get the address of index first */
		watchpoint = &watchpoints[slot + i];
		/* if the watchpoint is what we want, consume it. */
		if (atomic_compare_exchange_strong(
			    watchpoint, &addr_masked,
			    addr_masked | WATCHPOINT_CONSUMED_MASK))
			return watchpoint;
	}

	/* The corresponding watchpoint doesn't created. */
	return NULL;
}

static noinline void setup_watchpoint(const volatile void *ptr, size_t size,
				      int type, unsigned long ip)
{
	atomic_long *watchpoint = NULL;
	unsigned long old, new;

	/* insert the wp */
	old = encode_watchpoint((unsigned long)ptr, size,
				type_expect_write(type));
	watchpoint = insert_watchpoint((unsigned long)ptr, size,
				       type_expect_write(type));

	/* delay 80 ms, which reference from linux kernel default config */
	usleep(80U);

	/* check the wp and report */
	new = atomic_load(watchpoint);
	if (old ^ new)
		unify_report(ptr, size, type, ip, old, new,
			     (old | WATCHPOINT_CONSUMED_MASK) & new);

	/* remove the wp */
	atomic_store(watchpoint, 0);
}

static __always_inline void check_access(const volatile void *ptr, size_t size,
					 int type, unsigned long ip)
{
	atomic_long *watchpoint;

	/*
	 * If the watchpoint found, it will consume the wacthpoint (set the
	 *  WATCHPOINT_CONSUMED_MASK flag).
	 *  Otherwise it will return NULL.
	 */
	watchpoint = find_watchpoint((unsigned long)ptr, size,
				     type_expect_write(type));

	/*
	 * TODO: determine the watchpoint created.
	 * ANd handle the data race detect.
	 */

	/*
	 * Since we don't let the volatile access (which will be atomic
	 * type) to call the check_access(). It doesn't need to check
	 * the type we access is enable to create the watchpoint.
	 * In other words, we ignore the check of atomic operation
	 * (volatile access here), which means we don't do the
	 * reordering check.
	 */
	if (likely(watchpoint == NULL))
		setup_watchpoint(ptr, size, type, ip);
	else
		unify_set_info(ptr, size, type, ip, watchpoint - watchpoints);
}

/*
 * tsan function - export to the compiler
 */

#define DEFINE_TSAN_READ_WRITE(size)                                     \
	void __tsan_read##size(void *ptr);                               \
	void __tsan_read##size(void *ptr)                                \
	{                                                                \
		check_access(ptr, size, 0, _RET_IP_);                    \
	}                                                                \
	void __tsan_unaligned_read##size(void *ptr)                      \
		__alias(__tsan_read##size);                              \
	void __tsan_write##size(void *ptr);                              \
	void __tsan_write##size(void *ptr)                               \
	{                                                                \
		check_access(ptr, size, UCSAN_ACCESS_WRITE, _RET_IP_);   \
	}                                                                \
	void __tsan_unaligned_write##size(void *ptr)                     \
		__alias(__tsan_write##size);                             \
	void __tsan_read_write##size(void *ptr);                         \
	void __tsan_read_write##size(void *ptr)                          \
	{                                                                \
		check_access(ptr, size,                                  \
			     UCSAN_ACCESS_COMPOUND | UCSAN_ACCESS_WRITE, \
			     _RET_IP_);                                  \
	}                                                                \
	void __tsan_unaligned_read_write##size(void *ptr)                \
		__alias(__tsan_read_write##size);

DEFINE_TSAN_READ_WRITE(1);
DEFINE_TSAN_READ_WRITE(2);
DEFINE_TSAN_READ_WRITE(4);
DEFINE_TSAN_READ_WRITE(8);
DEFINE_TSAN_READ_WRITE(16);

#define DEFINE_TSAN_VOLATILE_READ_WRITE(size)                 \
	void __tsan_volatile_read##size(void *ptr);           \
	void __tsan_volatile_read##size(void *ptr)            \
	{                                                     \
		return;                                       \
	}                                                     \
	void __tsan_unaligned_volatile_read##size(void *ptr)  \
		__alias(__tsan_volatile_read##size);          \
	void __tsan_volatile_write##size(void *ptr);          \
	void __tsan_volatile_write##size(void *ptr)           \
	{                                                     \
		return;                                       \
	}                                                     \
	void __tsan_unaligned_volatile_write##size(void *ptr) \
		__alias(__tsan_volatile_write##size);

DEFINE_TSAN_VOLATILE_READ_WRITE(1);
DEFINE_TSAN_VOLATILE_READ_WRITE(2);
DEFINE_TSAN_VOLATILE_READ_WRITE(4);
DEFINE_TSAN_VOLATILE_READ_WRITE(8);
DEFINE_TSAN_VOLATILE_READ_WRITE(16);

void __tsan_func_entry(void *call_pc)
{
}

void __tsan_func_exit(void *p)
{
}

void __tsan_init(void)
{
}

#define __UT_DETECT
#include <unit_test.h>
