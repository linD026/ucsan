#ifndef __UCSAN_UCSAN_H__
#define __UCSAN_UCSAN_H__

#include <stdatomic.h>

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
 * We use the following macro as:
 * 	- src/core.c: atomic_long watchpoints[NR_UCSAN_SLOT * NR_UCSAN_WP];
 *	- src/unify.c: struct cached_info cached_info[NR_UCSAN_SLOT * NR_UCSAN_WP];
 *
 * NOTE: The UCSAN_NR_WATCHPOINT is from config, we won't use it directly.
 */
#define NR_UCSAN_SLOT UCSAN_NR_WATCHPOINT
#define NR_UCSAN_WP (PAGE_SIZE / sizeof(atomic_long))
#define WP_SLOT(addr) ((addr >> PAGE_SHIFT) % NR_UCSAN_SLOT)

#endif /* __UCSAN_UCSAN_H__ */
