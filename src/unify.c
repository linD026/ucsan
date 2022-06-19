#include <ucsan/unify.h>
#include <ucsan/report.h>
#include <ucsan/ucsan.h>
#include <ucsan/compiler.h>
#include <ucsan/list.h>
#include <ucsan/spinlock.h>
#include <stdbool.h>
#include <stdlib.h>
#include <execinfo.h>

#define STACK_BUF_SIZE 16

struct task_info {
	pid_t pid;
	int cpuid;
	int access_type; /* read:true(1), write:false(0) */
	void *ptr; /* Address of data race object */
	size_t size; /* size of object */
	unsigned long ip;
	char **stack_info;
	int nr_stack_info; /* the number of string in stack_info */
	struct list_head head; /* head of the data race task */
};

static DEFINE_SPINLOCK(task_container_lock);
struct task_info *task_container[NR_UCSAN_SLOT * NR_UCSAN_WP] = { NULL };

int task_pid(void)
{
	return 0;
}

int smp_processor_id()
{
	return 0;
}

static __always_inline void collect_stack_info(char ***stack_info, int *nptrs)
{
	void *buf[STACK_BUF_SIZE];

	*nptrs = backtrace(buf, STACK_BUF_SIZE);
	*stack_info = backtrace_symbols(buf, *nptrs);
}

static int unify_task_container_producer(void *ptr, size_t size,
					 int access_type, unsigned long ip)
{
	const unsigned long slot = WP_SLOT((unsigned long)ptr);
	int i, ret = -1;
	struct task_info **task_con_pos = NULL;
	struct task_info *task = malloc(sizeof(struct task_info));
	if (!task)
		return ret;

	task->pid = task_pid();
	task->cpuid = smp_processor_id();
	task->access_type = access_type;
	task->ptr = ptr;
	task->size = size;
	task->ip = ip;
	collect_stack_info(&task->stack_info, &task->nr_stack_info);
	list_init(&task->head);

	spin_lock(&task_container_lock);

	for (i = 0; i < NR_UCSAN_WP; i++) {
		task_con_pos = &task_container[slot + i];

		if (!(*task_con_pos)) {
			*task_con_pos = task;
			ret = 0;
			break;
		} else if ((*task_con_pos)->ptr == task->ptr &&
			   (*task_con_pos)->size == task->size) {
			// TODO: handle the range of object
			list_add(&task->head, &(*task_con_pos)->head);
			ret = 0;
			break;
		}
	}

	spin_unlock(&task_container_lock);
	return ret;
}

static struct task_info *unify_task_conatainer_consumer(void *ptr, size_t size)
{
	const unsigned long slot = WP_SLOT((unsigned long)ptr);
	int i;
	struct task_info **task_con_pos = NULL;
	struct task_info *task = NULL;

	spin_lock(&task_container_lock);
	for (i = 0; i < NR_UCSAN_WP; i++) {
		task_con_pos = &task_container[slot + i];
		if (*task_con_pos && (*task_con_pos)->ptr == ptr &&
		    (*task_con_pos)->size == size) {
			task = *task_con_pos;
			*task_con_pos = NULL;
			break;
		}
	}
	spin_unlock(&task_container_lock);
	return task;
}

void unify_set_info(const volatile void *ptr, size_t size, int access_type,
		    unsigned long ip, int watchpoint_idx)
{
	if (unify_task_container_producer((void *)ptr, size, access_type, ip))
		printf("task container is Full\n");
}

void unify_report(const volatile void *ptr, size_t size, int type,
		  unsigned long ip, unsigned long old_value,
		  unsigned long new_value)
{
	int i;
	struct list_head *pos, *n;
	struct task_info current, *task,
		*head_task = unify_task_conatainer_consumer((void *)ptr, size);
	if (!head_task) {
		printf("unify report(): cannot find the task\n");
		return;
	}

	report_init();

	report_print("BUG: UCSAN: data-race in ");
	list_for_each (pos, &head_task->head) {
		task = container_of(pos, struct task_info, head);
		report_print("%p / ", (void *)task->ip);
	}
	report_print("%p / %p\n\n", (void *)ip, (void *)head_task->ip);

#define report_task_print(task)                                               \
	report_print("%s to 0x%px of %zu bytes by %d on cpu %d:\n",           \
		     type_expect_write(task->access_type) ? "write" : "read", \
		     task->ptr, task->size, task->pid, task->cpuid);          \
	for (i = 0; i < task->nr_stack_info; i++)                             \
		report_print("  %s\n", task->stack_info[i]);                  \
	report_print("\n");

	current.pid = task_pid();
	current.cpuid = smp_processor_id();
	current.access_type = type;
	current.ptr = (void *)ptr;
	current.size = size;
	current.ip = ip;
	collect_stack_info(&current.stack_info, &current.nr_stack_info);
	report_task_print((&current));

	report_task_print(head_task);

	list_for_each (pos, &head_task->head) {
		task = container_of(pos, struct task_info, head);
		report_task_print(task);
	}

#undef report_task_print

	report_print("value changed: 0x%0lx -> 0x%0lx\n\n", old_value,
		     new_value);

	report_exit();

	list_for_each_safe (pos, n, &head_task->head) {
		task = container_of(pos, struct task_info, head);
		list_del(&task->head);
		free(task->stack_info);
		free(task);
	}
	free(head_task);
	free(current.stack_info);
}

#define __UT_UNIFY
#include <unit_test.h>
