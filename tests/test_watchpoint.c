#ifdef __UT_DETECT
#include <stdbool.h>
#include <stdatomic.h>
#include <threads.h>
#include <ucsan/encoding.h>
#include <ucsan/ucsan.h>

#include "printut.h"

#undef __FILE_NAME__
#define __FILE_NAME__ "tests/test_watchpoint.c"

extern atomic_long watchpoints[NR_UCSAN_SLOT * NR_UCSAN_WP];

void unify_set_info(const volatile void *ptr, size_t size, int access_type,
		    unsigned long ip, int watchpoint_idx)
{
}

void unify_report(const volatile void *ptr, size_t size, int type,
		  unsigned long ip, unsigned long old, unsigned long new,
		  bool changed)
{
	unsigned long encoded = encode_watchpoint((unsigned long)ptr, size,
						  type_expect_write(type));
	pr_info("ptr=%p, size=%zu, type=%d, ip=%lu, old=%lx, new=%lx, changed=%d\n",
		ptr, size, type, ip, old, new, changed);

	if (encoded != old)
		pr_err("encoded(%lx) != old(%lx)\n", encoded, old);
	if (old == new)
		pr_err("old(%lx) == new(%lx)\n", old, new);
	if (!(changed && (old | WATCHPOINT_CONSUMED_MASK) & new))
		pr_err("old(%lx) | WATCHPOINT_CONSUMED_MASK (%lx) != new(%lx)\n",
		       old, old | WATCHPOINT_CONSUMED_MASK, new);
}

struct test_setup_report_struct {
	void *ptr;
	size_t size;
	int type;
};

int test_setup_report_thread(void *arg)
{
	struct test_setup_report_struct *tsrs = arg;
	atomic_long *watchpoint;

again:
	watchpoint = find_watchpoint((unsigned long)tsrs->ptr, tsrs->size,
				     type_expect_write(tsrs->type));
	if (watchpoint == NULL)
		goto again;

	pr_info("ptr=%p size=%zu type=%x, watchpoint=(idx=%ld,%p)\n", tsrs->ptr,
		tsrs->size, tsrs->type, watchpoint - watchpoints,
		(void *)watchpoint);

	return 0;
}

static int test_setup_report(void)
{
	int foo;
	void *ptr = &foo;
	size_t size = sizeof(__typeof__(foo));
	int type = UCSAN_ACCESS_WRITE;
	thrd_t p;
	struct test_setup_report_struct *tsrs =
		malloc(sizeof(struct test_setup_report_struct));

	BUG_ON(tsrs == NULL);
	tsrs->ptr = ptr;
	tsrs->size = size;
	tsrs->type = type;
	thrd_create(&p, test_setup_report_thread, (void *)tsrs);

	pr_info("ptr=%p size=%zu type=%x\n", ptr, size, type);
	setup_watchpoint(ptr, size, type, 12345);

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
	atomic_long *watchpoint = NULL;
	atomic_long *found_wp = NULL;

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

	if (watchpoint != found_wp)
		pr_err("insert(idx=%ld,%p) != find(idx=%ld,%p)\n",
		       watchpoint - watchpoints, (void *)watchpoint,
		       found_wp - watchpoints, (void *)found_wp);

	new = atomic_load(found_wp);
	if ((old | WATCHPOINT_CONSUMED_MASK) != new)
		pr_err("inserted but not found, old=%lx, new=%lx\n", old, new);

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
