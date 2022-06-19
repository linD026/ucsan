#ifndef __UCSAN_UNIFY_H__
#define __UCSAN_UNIFY_H__

#include <stddef.h>
#include <stdbool.h>

void unify_set_info(const volatile void *ptr, size_t size, int access_type,
		    unsigned long ip, int watchpoint_idx);
void unify_report(const volatile void *ptr, size_t size, int type,
		  unsigned long ip, unsigned long old_value,
		  unsigned long new_value);

#endif /* __UCSAN_REPORT_H__ */
