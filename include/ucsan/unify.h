#ifndef __UCSAN_UNIFY_H__
#define __UCSAN_UNIFY_H__

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

struct access_info {
	const volatile void *ptr;
	size_t size;
	int access_type;
	int task_pid;
	int cpu_id;
	unsigned long ip;
};

void unify_set_info(const volatile void *ptr, size_t size, int access_type,
		     unsigned long ip, int watchpoint_idx);

#ifdef __cplusplus
}
#endif

#endif /* __UCSAN_REPORT_H__ */
