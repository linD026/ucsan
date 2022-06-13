#include <ucsan/unify.h>
#include <ucsan/ucsan.h>

// TODO: Should we use the cpp file?

struct cached_info {
	/* TODO: Add the data structure to handle mutiple items */
	int dummy;
};

struct cached_info cached_info[NR_UCSAN_SLOT * NR_UCSAN_WP];

// TODO: OS specific
int task_pid(void)
{
	return 0;
}

// TODO: OS specific
int smp_processor_id()
{
	return 0;
}

// TODO
static void unify_add_info(struct access_info *ai, struct cached_info *ci)
{
}

void unify_set_info(const volatile void *ptr, size_t size, int access_type,
		     unsigned long ip, int watchpoint_idx)
{
	/* create access_info structure */
	struct access_info ai = {
		.ptr = ptr,
		.size = size,
		.access_type = access_type,
		.task_pid = task_pid(),
		.cpu_id = smp_processor_id(),
		.ip = ip,
	};

	/* Add to the structure */
	unify_add_info(&ai, &cached_info[watchpoint_idx]);
}
