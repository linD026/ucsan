#ifdef __UT_DETECT
#include <stdbool.h>
#include <stdatomic.h>
#include <threads.h>
#include <ucsan/encoding.h>
#include <ucsan/ucsan.h>

#include "printut.h"

#undef __FILE_NAME__
#define __FILE_NAME__ "tests/test_watchpoint.c"

extern atomic_ulong watchpoints[NR_UCSAN_SLOT * NR_UCSAN_WP];

static int test_setup_foo;
struct test_setup_report_struct {
	void *ptr;
	size_t size;
	int type;
	unsigned long ip;
};
static struct test_setup_report_struct tsrs, unify_tsrs;

void unify_set_info(const volatile void *ptr, size_t size, int access_type,
		    unsigned long ip, int watchpoint_idx)
{
	unify_tsrs.ptr = (void *)ptr;
	unify_tsrs.size = size;
	unify_tsrs.type = access_type;
	unify_tsrs.ip = ip;
}

void unify_report(const volatile void *ptr, size_t size, int type,
		  unsigned long ip, unsigned long old_value,
		  unsigned long new_value)
{
	unsigned long encoded = encode_watchpoint((unsigned long)ptr, size,
						  type_expect_write(type));

	pr_info("ptr=%p, size=%zu, type=%d, ip=%lu, old_value=%lx, new_value=%lx, encoded_watchpoint=%lx\n",
		ptr, size, type, ip, old_value, new_value, encoded);

#define check_match(utsrs_val, val, fmt)                                   \
	do {                                                               \
		if (unify_tsrs.utsrs_val != val) {                         \
			pr_err("unify_tsrs.utsrs_val(" fmt ") != val(" fmt \
			       ")\n",                                      \
			       unify_tsrs.utsrs_val, val);                 \
			BUG_ON(1);                                         \
		}                                                          \
	} while (0)

	check_match(ptr, ptr, "%p");
	check_match(size, size, "%zu");
	check_match(ip, ip, "%lx");
	check_match(type, type, "%x");

#undef check_match
	if (old_value == new_value) {
		pr_err("old_value(%lx) == new_value(%lx)\n", old_value,
		       new_value);
		BUG_ON(1);
	}
}

static int test_setup_report_thread(void *arg)
{
	struct test_setup_report_struct *tsrs = arg;
	atomic_ulong *watchpoint;
	int idx;

	*(int *)tsrs->ptr = 1;

again:
	watchpoint = find_watchpoint((unsigned long)tsrs->ptr, tsrs->size,
				     type_expect_write(tsrs->type));
	if (watchpoint == NULL)
		goto again;

	idx = watchpoint - watchpoints;

	pr_info("ptr=%p size=%zu type=%x, watchpoint=(idx=%d,%p)\n", tsrs->ptr,
		tsrs->size, tsrs->type, idx, (void *)watchpoint);

	unify_set_info(tsrs->ptr, tsrs->size, tsrs->type, tsrs->ip, idx);

	return 0;
}

static int test_setup_report(void)
{
	void *ptr = &test_setup_foo;
	const size_t size = sizeof(__typeof__(test_setup_foo));
	const int type = UCSAN_ACCESS_WRITE;
	const unsigned long ip = 12345;
	thrd_t p;

	tsrs.ptr = ptr;
	tsrs.size = size;
	tsrs.type = type;
	tsrs.ip = ip;

	thrd_create(&p, test_setup_report_thread, (void *)&tsrs);

	pr_info("ptr=%p size=%zu type=%x\n", ptr, size, type);
	setup_watchpoint(ptr, size, type, ip);

	thrd_join(p, NULL);

	return 0;
}

static int test_insert_find_watchpoint(void)
{
	int foo;
	void *ptr = &foo;
	size_t size = sizeof(__typeof__(foo));
	int type = UCSAN_ACCESS_WRITE;
	unsigned long old = 0, new = 0;
	atomic_ulong *watchpoint = NULL;
	atomic_ulong *found_wp = NULL;

	old = encode_watchpoint((unsigned long)ptr, size,
				type_expect_write(type));
	watchpoint = insert_watchpoint((unsigned long)ptr, size,
				       type_expect_write(type));
	BUG_ON(watchpoint == NULL);

	pr_info("ptr=%p size=%zu type=%x watchpoint=%p\n", ptr, size, type,
		(void *)watchpoint);

	found_wp = find_watchpoint((unsigned long)ptr, size,
				   type_expect_write(type));
	BUG_ON(found_wp == NULL);

	if (watchpoint != found_wp) {
		pr_err("insert(idx=%ld,%p) != find(idx=%ld,%p)\n",
		       watchpoint - watchpoints, (void *)watchpoint,
		       found_wp - watchpoints, (void *)found_wp);
		return -1;
	}

	new = atomic_load(found_wp);
	if (old != new) {
		pr_err("inserted but not found, old=%lx, new=%lx\n", old, new);
		return -1;
	}

	pr_info("watchpoint=%p old=%lx new=%lx\n", (void *)watchpoint, old,
		new);

	atomic_store(watchpoint, 0);
	return 0;
}

int main(void)
{
	UNIT_BUG_ON(test_insert_find_watchpoint());
	UNIT_BUG_ON(test_setup_report());

	return 0;
}
#endif /* __UT_DETECT */
