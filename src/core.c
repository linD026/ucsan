#define _GNU_SOURCE
#include <ucsan/compiler.h>
#include <uapi/ucsan.h>
#include <stdio.h>

#define UCSAN_ACCESS_READ 0x0
#define UCSAN_ACCESS_WRITE 0x1
#define UCSAN_ACCESS_COMPOUND 0x2

static __always_inline void check_access(const volatile void *ptr, size_t size,
					 int type, unsigned long ip)
{
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
