#ifndef __UCSAN_UNIFY_H__
#define __UCSAN_UNIFY_H__

#include <stddef.h>
#include <stdbool.h>

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
void unify_report(const volatile void *ptr, size_t size, int type,
		  unsigned long ip, unsigned long old, unsigned long new,
		  bool changed);

#ifdef __cplusplus
}
#endif

#endif /* __UCSAN_REPORT_H__ */
