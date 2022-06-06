#define _GNU_SOURCE
#include <ucsan/compiler.h>
#include <ucsan/encoding.h>
#include <uapi/ucsan.h>
#include <stdio.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>

#define UCSAN_ACCESS_READ 0x0
#define UCSAN_ACCESS_WRITE 0x1
#define UCSAN_ACCESS_COMPOUND 0x2

/* In Linux, the page size is 4096 bytes. */
#define PAGE_SIZE 4096
#define PAGE_SHIFT 12

/*
 * We first use the page size and address to get the slot of the address reside.
 * Then search all the index of that slot to get the corresponding watchpoint.
 * So the watchpoint data structure, will be like:
 *
 *			      NR_UCSAN_WP (generally is 4096 bytes size)
 *	 		slot 1 [0, 1, 2]
 * 	NR_UCSAN_SLOT	slot 2 [3, 4, 5]
 *  			and so on...
 *
 * NOTE: The UCSAN_NR_WATCHPOINT is from config, we won't use it directly.
 */
#define NR_UCSAN_SLOT UCSAN_NR_WATCHPOINT
#define NR_UCSAN_WP (PAGE_SIZE / sizeof(atomic_long))
#define WP_SLOT(addr) ((addr >> PAGE_SHIFT) % NR_UCSAN_SLOT)
atomic_long watchpoints[NR_UCSAN_SLOT * NR_UCSAN_WP];

static __always_inline atomic_long *
find_watchpoint(unsigned long addr, size_t size, bool expect_write)
{
	const unsigned long slot = WP_SLOT(addr);
	unsigned long addr_masked = 0;
	atomic_long *watchpoint = NULL;
	int i;

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
				     (type == UCSAN_ACCESS_WRITE |
				      UCSAN_ACCESS_COMPOUND));

	/*
	 * TODO: determine the watchpoint created.
	 * ANd handle the data race detect.
	 */
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

void __tsan_func_entry(void *call_pc)
{
}

void __tsan_func_exit(void *p)
{
}

void __tsan_init(void)
{
}
