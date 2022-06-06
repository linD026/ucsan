#ifndef __UCSAN_ENCODING_H__
#define __UCSAN_ENCODING_H__

/* For 64-bit architecture, the sizeof(long) is 8 bytes */
#define BITS_PER_LONG 64

/*
 * currently for 64-bit architecture, the max of address will be 57 bits from 5
 * level page table of virtual address.
 * So we can use the higher than 56 bit of address to specific the following:
 *
 *   - tasn read/write operation : 1 bit
 *   - object size		 : 5 bits for 1,2,4,8,16 size
 *   - consumed			 : 1 bit
 */
#define WATCHPOINT_ADDR_MASK ((1UL << 56) - 1)
#define WATCHPOINT_CONSUMED_MASK (1UL << 56)
#define WATCHPOINT_SIZE_1 (1UL << 57)
#define WATCHPOINT_SIZE_2 (1UL << 58)
#define WATCHPOINT_SIZE_4 (1UL << 59)
#define WATCHPOINT_SIZE_8 (1UL << 60)
#define WATCHPOINT_SIZE_16 (1UL << 61)
#define WATCHPOINT_WRITE_MASK (1UL << 62)

#endif /* __UCSAN_ENCODING_H__ */
