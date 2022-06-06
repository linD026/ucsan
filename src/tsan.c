#include <stdio.h>
#include <stdint.h>

static int32_t ucasn_depth = 0;

void tsan_read4(void *ptr)
{
	if (ucasn_depth == 0)
		ucasn_depth++;
	else
		return;
	printf("[TSAN READ] i32v, ucasn_depth=%d val=%d, addr=%p\n",
	       ucasn_depth, *(int32_t *)ptr, ptr);
}

void __tsan_func_entry(void *call_pc)
{
}

void __tsan_func_exit(void *p)
{
}

void __tsan_init(void)
{
}

void __tsan_read4(void *ptr)
{
	if (ucasn_depth == 0)
		ucasn_depth++;
	else
		return;
	printf("[TSAN READ] i32v, ucasn_depth=%d val=%d, addr=%p\n",
	       ucasn_depth, *(int32_t *)ptr, ptr);
}

void __tsan_write4(void *ptr)
{
	printf("[TSAN WRITE] i32v, ucasn_depth=%d val=%d, addr=%p\n",
	       ucasn_depth, *(int32_t *)ptr, ptr);
	ucasn_depth--;
}
